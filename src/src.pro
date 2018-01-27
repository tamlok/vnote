#-------------------------------------------------
#
# Project created by QtCreator 2016-10-01T11:03:59
#
#-------------------------------------------------

QT       += core gui webenginewidgets webchannel network svg printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VNote
TEMPLATE = app

RC_ICONS = resources/icons/vnote.ico
ICON = resources/icons/vnote.icns

TRANSLATIONS += translations/vnote_zh_CN.ts

SOURCES += main.cpp\
    vmainwindow.cpp \
    vdirectorytree.cpp \
    vnote.cpp \
    vnotebook.cpp \
    dialog/vnewdirdialog.cpp \
    vconfigmanager.cpp \
    vfilelist.cpp \
    dialog/vnewfiledialog.cpp \
    vedit.cpp \
    vdocument.cpp \
    utils/vutils.cpp \
    vpreviewpage.cpp \
    hgmarkdownhighlighter.cpp \
    vstyleparser.cpp \
    dialog/vnewnotebookdialog.cpp \
    vmarkdownconverter.cpp \
    dialog/vnotebookinfodialog.cpp \
    dialog/vdirinfodialog.cpp \
    dialog/vfileinfodialog.cpp \
    veditoperations.cpp \
    vmdeditoperations.cpp \
    dialog/vinsertimagedialog.cpp \
    vdownloader.cpp \
    veditarea.cpp \
    veditwindow.cpp \
    vedittab.cpp \
    voutline.cpp \
    vsingleinstanceguard.cpp \
    vdirectory.cpp \
    vfile.cpp \
    vnotebookselector.cpp \
    vnofocusitemdelegate.cpp \
    vmdedit.cpp \
    dialog/vfindreplacedialog.cpp \
    dialog/vsettingsdialog.cpp \
    dialog/vdeletenotebookdialog.cpp \
    dialog/vselectdialog.cpp \
    vcaptain.cpp \
    vopenedlistmenu.cpp \
    vnavigationmode.cpp \
    vorphanfile.cpp \
    vcodeblockhighlighthelper.cpp \
    vwebview.cpp \
    vexporter.cpp \
    vmdtab.cpp \
    vhtmltab.cpp \
    utils/vvim.cpp \
    utils/veditutils.cpp \
    vvimindicator.cpp \
    vbuttonwithwidget.cpp \
    vtabindicator.cpp \
    dialog/vupdater.cpp \
    dialog/vorphanfileinfodialog.cpp \
    vtextblockdata.cpp \
    utils/vpreviewutils.cpp \
    dialog/vconfirmdeletiondialog.cpp \
    vnotefile.cpp \
    vattachmentlist.cpp \
    dialog/vsortdialog.cpp \
    vfilesessioninfo.cpp \
    vtableofcontent.cpp \
    utils/vmetawordmanager.cpp \
    vmetawordlineedit.cpp \
    dialog/vinsertlinkdialog.cpp \
    vplaintextedit.cpp \
    vimageresourcemanager.cpp \
    vlinenumberarea.cpp \
    veditor.cpp \
    vmdeditor.cpp \
    veditconfig.cpp \
    vpreviewmanager.cpp \
    vimageresourcemanager2.cpp \
    vtextdocumentlayout.cpp \
    vtextedit.cpp \
    vsnippetlist.cpp \
    vsnippet.cpp \
    dialog/veditsnippetdialog.cpp \
    utils/vimnavigationforwidget.cpp \
    vtoolbox.cpp \
    vinsertselector.cpp \
    utils/vclipboardutils.cpp \
    vpalette.cpp \
    vbuttonmenuitem.cpp \
    utils/viconutils.cpp \
    lineeditdelegate.cpp \
    dialog/vtipsdialog.cpp \
    dialog/vcopytextashtmldialog.cpp \
    vwaitingwidget.cpp \
    utils/vwebutils.cpp \
    vlineedit.cpp \
    vcart.cpp \
    vvimcmdlineedit.cpp \
    vlistwidget.cpp

HEADERS  += vmainwindow.h \
    vdirectorytree.h \
    vnote.h \
    vnotebook.h \
    dialog/vnewdirdialog.h \
    vconfigmanager.h \
    vfilelist.h \
    dialog/vnewfiledialog.h \
    vedit.h \
    vconstants.h \
    vdocument.h \
    utils/vutils.h \
    vpreviewpage.h \
    hgmarkdownhighlighter.h \
    vstyleparser.h \
    dialog/vnewnotebookdialog.h \
    vmarkdownconverter.h \
    dialog/vnotebookinfodialog.h \
    dialog/vdirinfodialog.h \
    dialog/vfileinfodialog.h \
    veditoperations.h \
    vmdeditoperations.h \
    dialog/vinsertimagedialog.h \
    vdownloader.h \
    veditarea.h \
    veditwindow.h \
    vedittab.h \
    voutline.h \
    vsingleinstanceguard.h \
    vdirectory.h \
    vfile.h \
    vnotebookselector.h \
    vnofocusitemdelegate.h \
    vmdedit.h \
    dialog/vfindreplacedialog.h \
    dialog/vsettingsdialog.h \
    dialog/vdeletenotebookdialog.h \
    dialog/vselectdialog.h \
    vcaptain.h \
    vopenedlistmenu.h \
    vnavigationmode.h \
    vorphanfile.h \
    vcodeblockhighlighthelper.h \
    vwebview.h \
    vexporter.h \
    vmdtab.h \
    vhtmltab.h \
    utils/vvim.h \
    utils/veditutils.h \
    vvimindicator.h \
    vbuttonwithwidget.h \
    vedittabinfo.h \
    vtabindicator.h \
    dialog/vupdater.h \
    dialog/vorphanfileinfodialog.h \
    vtextblockdata.h \
    utils/vpreviewutils.h \
    dialog/vconfirmdeletiondialog.h \
    vnotefile.h \
    vattachmentlist.h \
    dialog/vsortdialog.h \
    vfilesessioninfo.h \
    vtableofcontent.h \
    utils/vmetawordmanager.h \
    vmetawordlineedit.h \
    dialog/vinsertlinkdialog.h \
    vplaintextedit.h \
    vimageresourcemanager.h \
    vlinenumberarea.h \
    veditor.h \
    vmdeditor.h \
    veditconfig.h \
    vpreviewmanager.h \
    vimageresourcemanager2.h \
    vtextdocumentlayout.h \
    vtextedit.h \
    vsnippetlist.h \
    vsnippet.h \
    dialog/veditsnippetdialog.h \
    utils/vimnavigationforwidget.h \
    vtoolbox.h \
    vinsertselector.h \
    utils/vclipboardutils.h \
    vpalette.h \
    vbuttonmenuitem.h \
    utils/viconutils.h \
    lineeditdelegate.h \
    dialog/vtipsdialog.h \
    dialog/vcopytextashtmldialog.h \
    vwaitingwidget.h \
    utils/vwebutils.h \
    vlineedit.h \
    vcart.h \
    vvimcmdlineedit.h \
    vlistwidget.h

RESOURCES += \
    vnote.qrc \
    translations.qrc

OTHER_FILES += \
    utils/highlightjs/highlight.pack.js \
    utils/markdown-it/markdown-it-headinganchor.js \
    utils/markdown-it/markdown-it-task-lists.min.js \
    utils/markdown-it/markdown-it.min.js \
    utils/marked/marked.min.js \
    utils/mermaid/mermaidAPI.min.js

macx {
    LIBS += -L/usr/local/lib
    INCLUDEPATH += /usr/local/include
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../hoedown/release/ -lhoedown
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../hoedown/debug/ -lhoedown
else:unix: LIBS += -L$$OUT_PWD/../hoedown/ -lhoedown

INCLUDEPATH += $$PWD/../hoedown
DEPENDPATH += $$PWD/../hoedown

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../peg-highlight/release/ -lpeg-highlight
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../peg-highlight/debug/ -lpeg-highlight
else:unix: LIBS += -L$$OUT_PWD/../peg-highlight/ -lpeg-highlight

INCLUDEPATH += $$PWD/../peg-highlight
DEPENDPATH += $$PWD/../peg-highlight

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../peg-highlight/release/libpeg-highlight.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../peg-highlight/debug/libpeg-highlight.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../peg-highlight/release/peg-highlight.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../peg-highlight/debug/peg-highlight.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../peg-highlight/libpeg-highlight.a

## INSTALLS
unix:!macx {
    isEmpty(PREFIX): PREFIX = /usr
    DATADIR = $${PREFIX}/share

    # install desktop file
    desktop.path = $${DATADIR}/applications
    desktop.files += vnote.desktop

    # install icons
    icon16.path = $${DATADIR}/icons/hicolor/16x16/apps
    icon16.files = resources/icons/16x16/vnote.png

    icon32.path = $${DATADIR}/icons/hicolor/32x32/apps
    icon32.files = resources/icons/32x32/vnote.png

    icon48.path = $${DATADIR}/icons/hicolor/48x48/apps
    icon48.files = resources/icons/48x48/vnote.png

    icon64.path = $${DATADIR}/icons/hicolor/64x64/apps
    icon64.files = resources/icons/64x64/vnote.png

    icon128.path = $${DATADIR}/icons/hicolor/128x128/apps
    icon128.files = resources/icons/128x128/vnote.png

    icon256.path = $${DATADIR}/icons/hicolor/256x256/apps
    icon256.files = resources/icons/256x256/vnote.png

    iconsvg.path = $${DATADIR}/icons/hicolor/scalable/apps
    iconsvg.files = resources/icons/vnote.svg

    target.path = $${PREFIX}/bin

    INSTALLS += target desktop icon16 icon32 icon48 icon64 icon128 icon256 iconsvg
    message("VNote will be installed in prefix $${PREFIX}")
}
