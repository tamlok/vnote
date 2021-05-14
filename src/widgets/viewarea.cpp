#include "viewarea.h"

#include <QShortcut>
#include <QLabel>
#include <QLayout>
#include <QSize>
#include <QSplitter>
#include <QCoreApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QTimer>
#include <QApplication>
#include <QSet>
#include <QHash>

#include "viewwindow.h"
#include "mainwindow.h"
#include "events.h"
#include <utils/widgetutils.h>
#include <utils/docsutils.h>
#include <utils/urldragdroputils.h>
#include <core/vnotex.h>
#include <core/configmgr.h>
#include <core/coreconfig.h>
#include <core/sessionconfig.h>
#include <core/fileopenparameters.h>
#include <notebook/node.h>
#include <notebook/notebook.h>

using namespace vnotex;

ViewArea::ViewArea(QWidget *p_parent)
    : QWidget(p_parent),
      NavigationMode(NavigationMode::Type::DoubleKeys, this)
{
    setupUI();

    setAcceptDrops(true);

    setupShortcuts();

    connect(this, &ViewArea::viewSplitsCountChanged,
            this, &ViewArea::handleViewSplitsCountChange);

    auto mainWindow = VNoteX::getInst().getMainWindow();
    connect(mainWindow, &MainWindow::mainWindowClosed,
            this, [this](const QSharedPointer<Event> &p_event) {
                if (p_event->m_handled) {
                    return;
                }

                if (ConfigMgr::getInst().getCoreConfig().isRecoverLastSessionOnStartEnabled()) {
                    saveSession();
                }

                bool ret = close(false);
                if (!ret) {
                    p_event->m_response = false;
                    p_event->m_handled = true;
                }
            });

    connect(mainWindow, &MainWindow::mainWindowClosedOnQuit,
            this, [this]() {
                close(true);
            });

    if (ConfigMgr::getInst().getCoreConfig().isRecoverLastSessionOnStartEnabled()) {
        connect(mainWindow, &MainWindow::mainWindowStarted,
                this, &ViewArea::loadSession);
    }

    connect(&VNoteX::getInst(), &VNoteX::nodeAboutToMove,
            this, &ViewArea::handleNodeChange);

    connect(&VNoteX::getInst(), &VNoteX::nodeAboutToRemove,
            this, &ViewArea::handleNodeChange);

    connect(&VNoteX::getInst(), &VNoteX::nodeAboutToRename,
            this, &ViewArea::handleNodeChange);

    connect(&VNoteX::getInst(), &VNoteX::nodeAboutToReload,
            this, &ViewArea::handleNodeChange);

    auto &configMgr = ConfigMgr::getInst();
    connect(&configMgr, &ConfigMgr::editorConfigChanged,
            this, [this]() {
                forEachViewWindow([](ViewWindow *p_win) {
                    p_win->handleEditorConfigChange();
                    return true;
                });
            });

    m_fileCheckTimer = new QTimer(this);
    m_fileCheckTimer->setSingleShot(false);
    m_fileCheckTimer->setInterval(2000);
    connect(m_fileCheckTimer, &QTimer::timeout,
            this, [this]() {
                auto win = getCurrentViewWindow();
                if (win) {
                    win->checkFileMissingOrChangedOutsidePeriodically();
                }
            });

    connect(qApp, &QApplication::focusChanged,
            this, [this](QWidget *p_old, QWidget *p_now) {
                if (!p_now) {
                    m_fileCheckTimer->stop();
                } else if (!p_old && m_currentSplit) {
                    m_fileCheckTimer->start();
                }
            });
}

ViewArea::~ViewArea()
{
    // All splits/workspaces/windows should be released during close() before destruction.
    Q_ASSERT(m_splits.isEmpty() && m_currentSplit == nullptr);
    Q_ASSERT(m_workspaces.isEmpty());
}

void ViewArea::handleNodeChange(Node *p_node, const QSharedPointer<Event> &p_event)
{
    if (p_event->m_handled) {
        return;
    }

    bool ret = close(p_node, false);
    p_event->m_response = ret;
    p_event->m_handled = !ret;
}

void ViewArea::setupUI()
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
}

QSize ViewArea::sizeHint() const
{
    const QSize preferredSize(400, 300);
    auto sz = QWidget::sizeHint();
    if (sz.width() < preferredSize.width()) {
        sz = preferredSize;
    }

    return sz;
}

void ViewArea::openBuffer(Buffer *p_buffer, const QSharedPointer<FileOpenParameters> &p_paras)
{
    // We allow multiple ViewWindows of the same buffer in different workspaces by default.
    QVector<ViewWindow *> wins;
    if (!p_paras->m_alwaysNewWindow) {
        wins = findBufferInViewSplits(p_buffer);
    }
    if (wins.isEmpty()) {
        if (!m_currentSplit) {
            addFirstViewSplit();
        }

        Q_ASSERT(m_currentSplit);

        // Create a ViewWindow from @p_buffer.
        auto window = p_buffer->createViewWindow(p_paras, nullptr);
        m_currentSplit->addViewWindow(window);
        setCurrentViewWindow(window);
    } else {
        auto selectedWin = wins.first();
        for (auto win : wins) {
            // Prefer window in current split.
            if (win->getViewSplit() == m_currentSplit) {
                selectedWin = win;
                break;
            }
        }

        selectedWin->openTwice(p_paras);

        setCurrentViewWindow(selectedWin);
    }

    if (p_paras->m_focus) {
        auto win = getCurrentViewWindow();
        if (win) {
            win->setFocus(Qt::OtherFocusReason);
        }
    }
}

QVector<ViewWindow *> ViewArea::findBufferInViewSplits(const Buffer *p_buffer) const
{
    QVector<ViewWindow *> wins;
    for (auto split : m_splits) {
        auto winsInSplit = split->findBuffer(p_buffer);
        if (!winsInSplit.isEmpty()) {
            wins.append(winsInSplit);
        }
    }

    return wins;
}

ViewSplit *ViewArea::createViewSplit(QWidget *p_parent, ID p_viewSplitId)
{
    auto workspace = createWorkspace();
    m_workspaces.push_back(workspace);

    ID id = p_viewSplitId;
    if (id == InvalidViewSplitId) {
        id = m_nextViewSplitId++;
    } else {
        Q_ASSERT(p_viewSplitId >= m_nextViewSplitId);
        m_nextViewSplitId = id + 1;
    }

    auto split = new ViewSplit(m_workspaces, workspace, id, p_parent);
    connect(split, &ViewSplit::viewWindowCloseRequested,
            this, [this](ViewWindow *p_win) {
                closeViewWindow(p_win, false, true);
            });
    connect(split, &ViewSplit::verticalSplitRequested,
            this, [this](ViewSplit *p_split) {
                splitViewSplit(p_split, SplitType::Vertical);
            });
    connect(split, &ViewSplit::horizontalSplitRequested,
            this, [this](ViewSplit *p_split) {
                splitViewSplit(p_split, SplitType::Horizontal);
            });
    connect(split, &ViewSplit::maximizeSplitRequested,
            this, &ViewArea::maximizeViewSplit);
    connect(split, &ViewSplit::distributeSplitsRequested,
            this, &ViewArea::distributeViewSplits);
    connect(split, &ViewSplit::removeSplitRequested,
            this, [this](ViewSplit *p_split) {
                removeViewSplit(p_split, false);
            });
    connect(split, &ViewSplit::removeSplitAndWorkspaceRequested,
            this, [this](ViewSplit *p_split) {
                removeViewSplit(p_split, true);
            });
    connect(split, &ViewSplit::newWorkspaceRequested,
            this, &ViewArea::newWorkspaceInViewSplit);
    connect(split, &ViewSplit::removeWorkspaceRequested,
            this, [this](ViewSplit *p_split) {
                removeWorkspaceInViewSplit(p_split, true);
            });
    connect(split, &ViewSplit::focused,
            this, [this](ViewSplit *p_split) {
                setCurrentViewSplit(p_split, false);
                checkCurrentViewWindowChange();
            });
    connect(split, &ViewSplit::currentViewWindowChanged,
            this, [this](ViewWindow *p_win) {
                checkCurrentViewWindowChange();
                if (shouldUseGlobalStatusWidget()) {
                    if (p_win) {
                        p_win->setStatusWidgetVisible(false);
                    }
                    m_currentStatusWidget = p_win ? p_win->statusWidget() : nullptr;
                    emit statusWidgetChanged(m_currentStatusWidget.get());
                } else {
                    Q_ASSERT(!m_currentStatusWidget);
                    if (p_win) {
                        p_win->setStatusWidgetVisible(true);
                    }
                }
            });
    return split;
}

void ViewArea::showSceneWidget()
{
    Q_ASSERT(!m_sceneWidget);
    Q_ASSERT(m_splits.isEmpty());
    auto text = DocsUtils::getDocText(QStringLiteral("get_started.txt"));
    // TODO: a more informative widget, such as adding workspace list and LRU files.
    m_sceneWidget = new QLabel(text, this);
    m_mainLayout->addWidget(m_sceneWidget);
}

void ViewArea::hideSceneWidget()
{
    Q_ASSERT(m_sceneWidget);
    m_mainLayout->removeWidget(m_sceneWidget);
    delete m_sceneWidget;
    m_sceneWidget = nullptr;
}

void ViewArea::addFirstViewSplit()
{
    Q_ASSERT(!m_currentSplit && m_splits.isEmpty());

    auto split = createViewSplit(this);

    m_splits.push_back(split);

    hideSceneWidget();
    m_mainLayout->addWidget(split);

    postFirstViewSplit();
}

void ViewArea::postFirstViewSplit()
{
    Q_ASSERT(!m_splits.isEmpty());
    setCurrentViewSplit(m_splits.first(), false);

    emit viewSplitsCountChanged();
    checkCurrentViewWindowChange();

    m_fileCheckTimer->start();
}

static ViewSplit *fetchFirstChildViewSplit(const QSplitter *p_splitter)
{
    if (p_splitter->count() == 0) {
        return nullptr;
    }

    auto child = p_splitter->widget(0);
    auto split = dynamic_cast<ViewSplit *>(child);
    if (split) {
        return split;
    }

    auto childSplitter = dynamic_cast<QSplitter *>(child);
    Q_ASSERT(childSplitter);
    return fetchFirstChildViewSplit(childSplitter);
}

void ViewArea::removeViewSplit(ViewSplit *p_split, bool p_removeWorkspace)
{
    if (p_removeWorkspace) {
        // Remove workspace.
        bool ret = removeWorkspaceInViewSplit(p_split, false);
        if (!ret) {
            return;
        }
    } else {
        // Detach workspace.
        p_split->setWorkspace(nullptr);
    }

    // Remove split.
    disconnect(p_split, 0, this, 0);
    disconnect(this, 0, p_split, 0);
    m_splits.removeAll(p_split);

    // Get new current split.
    ViewSplit *newCurrentSplit = nullptr;
    auto splitter = tryGetParentSplitter(p_split);
    if (splitter) {
        Q_ASSERT(splitter->count() >= 2);
        p_split->hide();
        p_split->setParent(this);

        newCurrentSplit = fetchFirstChildViewSplit(splitter);

        if (splitter->count() == 1) {
            // Remove the splitter if there is only one child in it after the removal.
            unwrapSplitter(splitter);
        }
    } else {
        Q_ASSERT(m_splits.isEmpty());
        m_mainLayout->removeWidget(p_split);
        if (!m_splits.isEmpty()) {
            newCurrentSplit = m_splits.first();
        }
    }

    p_split->deleteLater();

    // Show scene widget and update current split.
    if (m_splits.isEmpty()) {
        Q_ASSERT(newCurrentSplit == nullptr);
        setCurrentViewSplit(newCurrentSplit, false);
        showSceneWidget();

        m_fileCheckTimer->stop();
    } else if (m_currentSplit == p_split) {
        setCurrentViewSplit(newCurrentSplit, true);
    }

    emit viewSplitsCountChanged();
    checkCurrentViewWindowChange();
}

ViewWindow *ViewArea::getCurrentViewWindow() const
{
    auto split = getCurrentViewSplit();
    if (split) {
        return split->getCurrentViewWindow();
    }

    return nullptr;
}

void ViewArea::setCurrentViewWindow(ViewWindow *p_win)
{
    auto split = p_win->getViewSplit();
    Q_ASSERT(split);

    split->setCurrentViewWindow(p_win);

    setCurrentViewSplit(split, false);

    checkCurrentViewWindowChange();
}

ViewSplit *ViewArea::getCurrentViewSplit() const
{
    return m_currentSplit;
}

void ViewArea::setCurrentViewSplit(ViewSplit *p_split, bool p_focus)
{
    Q_ASSERT(!p_split || m_splits.contains(p_split));
    if (p_split == m_currentSplit) {
        return;
    }

    if (m_currentSplit) {
        m_currentSplit->setActive(false);
    }

    m_currentSplit = p_split;
    if (m_currentSplit) {
        m_currentSplit->setActive(true);
        if (p_focus) {
            m_currentSplit->focus();
        }
    }
}

bool ViewArea::closeViewWindow(ViewWindow *p_win, bool p_force, bool p_removeSplitIfEmpty)
{
    Q_ASSERT(p_win && p_win->getViewSplit());
    // Make it current ViewWindow.
    setCurrentViewWindow(p_win);

    if (!p_win->aboutToClose(p_force)) {
        return false;
    }

    // Remove the status widget.
    if (m_currentStatusWidget && p_win == getCurrentViewWindow()) {
        Q_ASSERT(m_currentStatusWidget == p_win->statusWidget());
        emit statusWidgetChanged(nullptr);
    }

    auto split = p_win->getViewSplit();
    split->takeViewWindow(p_win);

    delete p_win;

    if (p_removeSplitIfEmpty && split->getViewWindowCount() == 0) {
        // Remove this split and workspace.
        removeViewSplit(split, true);
    }

    return true;
}

QSharedPointer<ViewWorkspace> ViewArea::createWorkspace()
{
    // Get the id of the workspace.
    ID id = 1;
    QSet<ID> usedIds;
    for (auto ws : m_workspaces) {
        usedIds.insert(ws->m_id);
    }

    while (true) {
        if (usedIds.contains(id)) {
            ++id;
        } else {
            break;
        }
    }

    return QSharedPointer<ViewWorkspace>::create(id);
}

void ViewArea::splitViewSplit(ViewSplit *p_split, SplitType p_type)
{
    Q_ASSERT(p_split);
    // Create the new split.
    auto newSplit = createViewSplit(this);

    // Clone a ViewWindow for the same buffer to display in the new split.
    {
        auto win = p_split->getCurrentViewWindow();
        if (win) {
            auto buffer = win->getBuffer();
            auto newWindow = buffer->createViewWindow(QSharedPointer<FileOpenParameters>::create(), newSplit);
            newSplit->addViewWindow(newWindow);
        }
    }

    // Obey Vim's practice, which is the opposite of Qt.
    auto orientation = p_type == SplitType::Vertical ? Qt::Horizontal : Qt::Vertical;
    auto splitter = tryGetParentSplitter(p_split);
    if (splitter) {
        int idx = splitter->indexOf(p_split);
        if (splitter->orientation() == orientation) {
            // Same orientation.
            splitter->insertWidget(idx + 1, newSplit);
        } else {
            // Split it further.
            auto newSplitter = createSplitter(orientation, this);
            splitter->replaceWidget(idx, newSplitter);

            newSplitter->addWidget(p_split);
            newSplitter->addWidget(newSplit);
        }
    } else {
        Q_ASSERT(p_split->parent() == this);
        m_mainLayout->removeWidget(p_split);

        auto newSplitter = createSplitter(orientation, this);
        newSplitter->addWidget(p_split);
        newSplitter->addWidget(newSplit);

        m_mainLayout->addWidget(newSplitter);
    }

    m_splits.push_back(newSplit);
    setCurrentViewSplit(newSplit, true);

    // Let Qt decide the size of splitter first.
    QCoreApplication::sendPostedEvents();

    distributeViewSplitsOfSplitter(tryGetParentSplitter(newSplit));

    emit viewSplitsCountChanged();
    checkCurrentViewWindowChange();
}

QSplitter *ViewArea::createSplitter(Qt::Orientation p_orientation, QWidget *p_parent) const
{
    auto splitter = new QSplitter(p_orientation, p_parent);
    splitter->setChildrenCollapsible(false);
    return splitter;
}

QSplitter *ViewArea::tryGetParentSplitter(const QWidget *p_widget) const
{
    return dynamic_cast<QSplitter *>(p_widget->parent());
}

void ViewArea::distributeViewSplitsOfSplitter(QSplitter *p_splitter)
{
    if (!p_splitter || p_splitter->count() <= 1) {
        return;
    }

    // Distribute the direct children of splitter.
    {
        auto sizes = p_splitter->sizes();
        int totalWidth = 0;
        for (auto sz : sizes) {
            totalWidth += sz;
        }

        int newWidth = totalWidth / sizes.size();
        if (newWidth <= 0) {
            return;
        }

        for (int i = 0; i < sizes.size(); ++i) {
            sizes[i] = newWidth;
        }

        p_splitter->setSizes(sizes);
    }

    // Distribute child splitter.
    for (int i = 0; i < p_splitter->count(); ++i) {
        auto childSplitter = dynamic_cast<QSplitter *>(p_splitter->widget(i));
        if (childSplitter) {
            distributeViewSplitsOfSplitter(childSplitter);
        }
    }

    return;
}

void ViewArea::unwrapSplitter(QSplitter *p_splitter)
{
    Q_ASSERT(p_splitter->count() == 1);
    auto paSplitter = tryGetParentSplitter(p_splitter);
    if (paSplitter) {
        Q_ASSERT(paSplitter->count() >= 2);
        int idx = paSplitter->indexOf(p_splitter);
        auto child = p_splitter->widget(0);
        child->setParent(this);
        paSplitter->replaceWidget(idx, child);
    } else {
        // This is the top child of ViewArea.
        Q_ASSERT(p_splitter->parent() == this);
        m_mainLayout->removeWidget(p_splitter);
        // Maybe another splitter or ViewSplit.
        auto child = p_splitter->widget(0);
        child->setParent(this);
        m_mainLayout->addWidget(child);
    }

    delete p_splitter;
}

void ViewArea::maximizeViewSplit(ViewSplit *p_split)
{
    QWidget *widget = p_split;
    while (widget && widget != this) {
        maximizeWidgetOfSplitter(widget);
        widget = dynamic_cast<QWidget *>(widget->parent());
    }
}

void ViewArea::maximizeWidgetOfSplitter(QWidget *p_widget)
{
    auto splitter = tryGetParentSplitter(p_widget);
    if (!splitter || splitter->count() <= 1) {
        return;
    }

    const int minSplitWidth = 20 * WidgetUtils::calculateScaleFactor();
    auto sizes = splitter->sizes();
    int totalWidth = 0;
    for (auto sz : sizes) {
        totalWidth += sz;
    }

    int newWidth = totalWidth - minSplitWidth * (sizes.size() - 1);
    if (newWidth <= 0) {
        return;
    }

    int idx = splitter->indexOf(p_widget);
    for (int i = 0; i < sizes.size(); ++i) {
        sizes[i] = (i == idx) ? newWidth : minSplitWidth;
    }

    splitter->setSizes(sizes);
}

void ViewArea::distributeViewSplits()
{
    // Get the top splitter if there is any.
    auto splitter = dynamic_cast<QSplitter *>(m_mainLayout->itemAt(0)->widget());
    if (!splitter) {
        return;
    }

    distributeViewSplitsOfSplitter(splitter);
}

void ViewArea::removeWorkspace(QSharedPointer<ViewWorkspace> p_workspace)
{
    if (!p_workspace) {
        return;
    }
    Q_ASSERT(!p_workspace->m_visible && p_workspace->m_viewWindows.isEmpty());

    p_workspace->clear();
    m_workspaces.removeAll(p_workspace);
}

void ViewArea::newWorkspaceInViewSplit(ViewSplit *p_split)
{
    auto workspace = createWorkspace();
    m_workspaces.push_back(workspace);

    p_split->setWorkspace(workspace);
    setCurrentViewSplit(p_split, true);
}

bool ViewArea::removeWorkspaceInViewSplit(ViewSplit *p_split, bool p_insertNew)
{
    // Close all the ViewWindows.
    setCurrentViewSplit(p_split, true);
    auto wins = getAllViewWindows(p_split);
    for (const auto win : wins) {
        if (!closeViewWindow(win, false, false)) {
            return false;
        }
    }

    Q_ASSERT(p_split->getViewWindowCount() == 0);
    auto workspace = p_split->getWorkspace();
    p_split->setWorkspace(nullptr);
    removeWorkspace(workspace);

    if (p_insertNew) {
        // Find an invisible workspace.
        bool found = false;
        for (auto &ws : m_workspaces) {
            if (!ws->m_visible) {
                p_split->setWorkspace(ws);
                found = true;
                break;
            }
        }

        // No invisible workspace. Create a new empty workspace.
        if (!found) {
            newWorkspaceInViewSplit(p_split);
        }
    }

    return true;
}

bool ViewArea::shouldUseGlobalStatusWidget() const
{
    return m_splits.size() <= 1;
}

void ViewArea::handleViewSplitsCountChange()
{
    if (shouldUseGlobalStatusWidget()) {
        // Hide the status widget for all ViewWindows.
        forEachViewWindow([](ViewWindow *p_win) {
            p_win->setStatusWidgetVisible(false);
            return true;
        });

        // Show global status widget for current ViewWindow.
        auto win = getCurrentViewWindow();
        m_currentStatusWidget = win ? win->statusWidget() : nullptr;
        emit statusWidgetChanged(m_currentStatusWidget.get());
    } else {
        // Show standalone status widget for all ViewWindows.
        emit statusWidgetChanged(nullptr);
        m_currentStatusWidget = nullptr;

        forEachViewWindow([](ViewWindow *p_win) {
            p_win->setStatusWidgetVisible(true);
            return true;
        });
    }
}

void ViewArea::forEachViewWindow(const ViewSplit::ViewWindowSelector &p_func)
{
    for (auto split : m_splits) {
        if (!split->forEachViewWindow(p_func)) {
            return;
        }
    }

    for (auto &ws : m_workspaces) {
        if (!ws->m_visible) {
            for (auto win : ws->m_viewWindows) {
                if (!p_func(win)) {
                    return;
                }
            }
        }
    }
}

bool ViewArea::close(bool p_force)
{
    return closeIf(p_force, [](ViewWindow *p_win) {
               Q_UNUSED(p_win);
               return true;
           }, true);
}

void ViewArea::setupShortcuts()
{
    const auto &coreConfig = ConfigMgr::getInst().getCoreConfig();

    // CloseTab.
    {
        auto shortcut = WidgetUtils::createShortcut(coreConfig.getShortcut(CoreConfig::CloseTab), this);
        if (shortcut) {
            connect(shortcut, &QShortcut::activated,
                    this, [this]() {
                        auto win = getCurrentViewWindow();
                        if (win) {
                            closeViewWindow(win, false, true);
                        }
                    });
        }
    }

    // LocateNode.
    {
        auto shortcut = WidgetUtils::createShortcut(coreConfig.getShortcut(CoreConfig::LocateNode), this);
        if (shortcut) {
            connect(shortcut, &QShortcut::activated,
                    this, [this]() {
                        auto win = getCurrentViewWindow();
                        if (win) {
                            auto node = win->getBuffer()->getNode();
                            if (node) {
                                emit VNoteX::getInst().locateNodeRequested(node);
                            }
                        }
                    });
        }
    }
}

bool ViewArea::close(Node *p_node, bool p_force)
{
    return closeIf(p_force, [p_node](ViewWindow *p_win) {
                auto buffer = p_win->getBuffer();
                return buffer->match(p_node) || buffer->isChildOf(p_node);
            }, false);
}

bool ViewArea::close(const Notebook *p_notebook, bool p_force)
{
    return close(p_notebook->getRootNode().data(), p_force);
}

void ViewArea::checkCurrentViewWindowChange()
{
    auto win = getCurrentViewWindow();
    if (win == m_currentWindow) {
        return;
    }

    m_currentWindow = win;
    emit currentViewWindowChanged();
}

bool ViewArea::closeIf(bool p_force, const ViewSplit::ViewWindowSelector &p_func, bool p_closeEmptySplit)
{
    // Go through all hidden workspace. Use current split to show the workspace.
    if (m_workspaces.size() > m_splits.size()) {
        if (!m_currentSplit) {
            // Create at least one split.
            addFirstViewSplit();
        }

        // Need to restore it.
        auto currentWorkspace = m_currentSplit->getWorkspace();

        QVector<QSharedPointer<ViewWorkspace>> hiddenWorkspaces;
        for (auto &ws : m_workspaces) {
            if (!ws->m_visible) {
                Q_ASSERT(ws != currentWorkspace);
                hiddenWorkspaces.push_back(ws);
            }
        }

        Q_ASSERT(!hiddenWorkspaces.isEmpty());
        for (auto &ws : hiddenWorkspaces) {
            m_currentSplit->setWorkspace(ws);

            // Go through this split.
            auto wins = getAllViewWindows(m_currentSplit, p_func);
            for (const auto win : wins) {
                // Do not remove the split even if it is empty.
                bool ret = closeViewWindow(win, p_force, false);
                if (!ret) {
                    // User cancels the close of one ViewWindow. No need to restore the workspace.
                    return false;
                }
            }

            m_currentSplit->setWorkspace(nullptr);

            // Remove this workspace if it is empty.
            if (ws->m_viewWindows.isEmpty()) {
                removeWorkspace(ws);
            }
        }

        // Restore.
        m_currentSplit->setWorkspace(currentWorkspace);
    }

    // Go through all splits.
    // Collect the ViewWindows first. Collect empty splits.
    QVector<ViewWindow *> wins;
    QVector<ViewSplit *> emptySplits;
    for (auto split : m_splits) {
        if (p_closeEmptySplit && split->getViewWindowCount() == 0) {
            emptySplits.push_back(split);
            continue;
        }

        wins.append(getAllViewWindows(split, p_func));
    }

    if (!emptySplits.isEmpty()) {
        // Remove empty splits.
        for (auto split : emptySplits) {
            removeViewSplit(split, true);
        }
    }

    if (wins.isEmpty()) {
        return true;
    }

    // Close the ViewWindow.
    for (auto win : wins) {
        bool ret = closeViewWindow(win, p_force, true);
        if (!ret) {
            return false;
        }
    }

    return true;
}

void ViewArea::focus()
{
    auto split = getCurrentViewSplit();
    if (split) {
        split->focus();
    }
}

QVector<void *> ViewArea::getVisibleNavigationItems()
{
    QVector<void *> items;
    m_navigationItems.clear();

    int idx = 0;
    for (auto split : m_splits) {
        if (split->getViewWindowCount() == 0) {
            continue;
        }
        if (idx >= NavigationMode::c_maxNumOfNavigationItems) {
            break;
        }
        auto info = split->getNavigationModeInfo();
        for (int i = 0; i < info.size() && idx < NavigationMode::c_maxNumOfNavigationItems; ++i, ++idx) {
            items.push_back(info[i].m_viewWindow);
            m_navigationItems.push_back(info[i]);
        }
    }
    return items;
}

void ViewArea::placeNavigationLabel(int p_idx, void *p_item, QLabel *p_label)
{
    Q_UNUSED(p_item);
    Q_ASSERT(p_idx > -1);
    p_label->setParent(m_navigationItems[p_idx].m_viewWindow->getViewSplit());
    p_label->move(m_navigationItems[p_idx].m_topLeft);
}

void ViewArea::handleTargetHit(void *p_item)
{
    if (p_item) {
        setCurrentViewWindow(static_cast<ViewWindow *>(p_item));
        focus();
    }
}

void ViewArea::clearNavigation()
{
    NavigationMode::clearNavigation();

    m_navigationItems.clear();
}

void ViewArea::dragEnterEvent(QDragEnterEvent *p_event)
{
    if (UrlDragDropUtils::handleDragEnterEvent(p_event)) {
        return;
    }

    QWidget::dragEnterEvent(p_event);
}

void ViewArea::dropEvent(QDropEvent *p_event)
{
    if (UrlDragDropUtils::handleDropEvent(p_event, [](const QStringList &p_files) {
                for (const auto &file : p_files) {
                    emit VNoteX::getInst().openFileRequested(file, QSharedPointer<FileOpenParameters>::create());
                }
            })) {
        return;
    }

    QWidget::dropEvent(p_event);
}

QVector<ViewWindow *> ViewArea::getAllViewWindows(ViewSplit *p_split, const ViewSplit::ViewWindowSelector &p_func) const
{
    QVector<ViewWindow *> wins;
    p_split->forEachViewWindow([p_func, &wins](ViewWindow *p_win) {
            if (p_func(p_win)) {
                wins.push_back(p_win);
            }
            return true;
        });
    return wins;
}

QVector<ViewWindow *> ViewArea::getAllViewWindows(ViewSplit *p_split) const
{
   return getAllViewWindows(p_split, [](ViewWindow *) {
              return true;
          });
}

QList<Buffer *> ViewArea::getAllBuffersInViewSplits() const
{
    QSet<Buffer *> bufferSet;

    for (auto split : m_splits) {
        auto wins = getAllViewWindows(split);
        for (auto win : wins) {
            bufferSet.insert(win->getBuffer());
        }
    }

    return bufferSet.values();
}

void ViewArea::loadSession()
{
    auto &sessionConfig = ConfigMgr::getInst().getSessionConfig();
    auto sessionData = sessionConfig.getViewAreaSessionAndClear();

    auto session = ViewAreaSession::deserialize(sessionData);

    // Load widgets layout.
    if (session.m_root.isEmpty()) {
        showSceneWidget();
    } else {
        Q_ASSERT(m_splits.isEmpty());
        if (session.m_root.m_type == ViewAreaSession::Node::Type::Splitter) {
            // Splitter.
            auto splitter = createSplitter(session.m_root.m_orientation, this);
            m_mainLayout->addWidget(splitter);

            loadSplitterFromSession(session.m_root, splitter);
        } else {
            // Just only one ViewSplit.
            Q_ASSERT(session.m_root.m_type == ViewAreaSession::Node::Type::ViewSplit);
            auto split = createViewSplit(this, session.m_root.m_viewSplitId);
            m_splits.push_back(split);
            m_mainLayout->addWidget(split);
        }

        QHash<ID, int> viewSplitToWorkspace;

        setCurrentViewSplit(m_splits.first(), false);

        // Load invisible workspace.
        for (int i = 0; i < session.m_workspaces.size(); ++i) {
            const auto &ws = session.m_workspaces[i];
            if (ws.m_viewSplitId != InvalidViewSplitId) {
                viewSplitToWorkspace.insert(ws.m_viewSplitId, i);
                continue;
            }

            for (const auto &winSession : ws.m_viewWindows) {
                openViewWindowFromSession(winSession);
            }

            // Check if there is any window.
            if (m_currentSplit->getViewWindowCount() > 0) {
                m_currentSplit->setCurrentViewWindow(ws.m_currentViewWindowIndex);

                // New another workspace.
                auto newWs = createWorkspace();
                m_workspaces.push_back(newWs);
                m_currentSplit->setWorkspace(newWs);
            }
        }

        // Load visible workspace.
        for (auto split : m_splits) {
            setCurrentViewSplit(split, false);

            auto it = viewSplitToWorkspace.find(split->getId());
            Q_ASSERT(it != viewSplitToWorkspace.end());

            const auto &ws = session.m_workspaces[it.value()];

            for (const auto &winSession : ws.m_viewWindows) {
                openViewWindowFromSession(winSession);
            }

            if (m_currentSplit->getViewWindowCount() > 0) {
                m_currentSplit->setCurrentViewWindow(ws.m_currentViewWindowIndex);
            }
        }

        postFirstViewSplit();

        distributeViewSplits();
    }
}

void ViewArea::saveSession() const
{
    ViewAreaSession session;
    takeSnapshot(session);

    auto &sessionConfig = ConfigMgr::getInst().getSessionConfig();
    sessionConfig.setViewAreaSession(session.serialize());
}

static void takeSnapshotOfWidgetNodes(ViewAreaSession::Node &p_node, const QWidget *p_widget, QHash<ID, ID> &p_workspaceToViewSplit)
{
    p_node.clear();

    // Splitter.
    auto splitter = dynamic_cast<const QSplitter *>(p_widget);
    if (splitter) {
        p_node.m_type = ViewAreaSession::Node::Type::Splitter;
        p_node.m_orientation = splitter->orientation();
        p_node.m_children.resize(splitter->count());

        for (int i = 0; i < p_node.m_children.size(); ++i) {
            takeSnapshotOfWidgetNodes(p_node.m_children[i], splitter->widget(i), p_workspaceToViewSplit);
        }

        return;
    }

    // ViewSplit.
    auto viewSplit = dynamic_cast<const ViewSplit *>(p_widget);
    Q_ASSERT(viewSplit);
    p_node.m_type = ViewAreaSession::Node::Type::ViewSplit;
    p_node.m_viewSplitId = viewSplit->getId();

    auto ws = viewSplit->getWorkspace();
    if (ws) {
        viewSplit->updateStateToWorkspace();
        p_workspaceToViewSplit.insert(ws->m_id, viewSplit->getId());
    }
}


void ViewArea::takeSnapshot(ViewAreaSession &p_session) const
{
    QHash<ID, ID> workspaceToViewSplit;

    // Widget hirarchy.
    p_session.m_root.clear();
    if (!m_splits.isEmpty()) {
        auto topWidget = m_mainLayout->itemAt(0)->widget();
        takeSnapshotOfWidgetNodes(p_session.m_root, topWidget, workspaceToViewSplit);
    }

    // Workspaces.
    p_session.m_workspaces.clear();
    p_session.m_workspaces.reserve(m_workspaces.size());
    for (const auto &ws : m_workspaces) {
        p_session.m_workspaces.push_back(ViewAreaSession::Workspace());
        auto &wsSnap = p_session.m_workspaces.last();
        if (ws->m_visible) {
            auto it = workspaceToViewSplit.find(ws->m_id);
            Q_ASSERT(it != workspaceToViewSplit.end());
            wsSnap.m_viewSplitId = it.value();
        }
        wsSnap.m_currentViewWindowIndex = ws->m_currentViewWindowIndex;
        for (auto win : ws->m_viewWindows) {
            wsSnap.m_viewWindows.push_back(win->saveSession());
        }
    }
}

void ViewArea::loadSplitterFromSession(const ViewAreaSession::Node &p_node, QSplitter *p_splitter)
{
    // @p_splitter is the splitter corresponding to @p_node.
    Q_ASSERT(p_node.m_type == ViewAreaSession::Node::Type::Splitter);

    for (const auto &child : p_node.m_children) {
        if (child.m_type == ViewAreaSession::Node::Type::Splitter) {
            auto childSplitter = createSplitter(child.m_orientation, p_splitter);
            p_splitter->addWidget(childSplitter);

            loadSplitterFromSession(child, childSplitter);
        } else {
            Q_ASSERT(child.m_type == ViewAreaSession::Node::Type::ViewSplit);
            auto childSplit = createViewSplit(this, child.m_viewSplitId);
            m_splits.push_back(childSplit);
            p_splitter->addWidget(childSplit);
        }
    }
}

void ViewArea::openViewWindowFromSession(const ViewWindowSession &p_session)
{
    if (p_session.m_bufferPath.isEmpty()) {
        return;
    }

    auto paras = QSharedPointer<FileOpenParameters>::create();
    paras->m_mode = p_session.m_viewWindowMode;
    paras->m_readOnly = p_session.m_readOnly;
    paras->m_lineNumber = p_session.m_lineNumber;
    paras->m_alwaysNewWindow = true;

    emit VNoteX::getInst().openFileRequested(p_session.m_bufferPath, paras);
}
