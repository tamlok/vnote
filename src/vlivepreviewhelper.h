#ifndef VLIVEPREVIEWHELPER_H
#define VLIVEPREVIEWHELPER_H

#include <QObject>
#include <QTextDocument>

#include "pegmarkdownhighlighter.h"
#include "vpreviewmanager.h"
#include "vconstants.h"

class VEditor;
class VDocument;
class VGraphvizHelper;
class VPlantUMLHelper;
class VMathJaxPreviewHelper;

class CodeBlockPreviewInfo
{
public:
    CodeBlockPreviewInfo();

    explicit CodeBlockPreviewInfo(const VCodeBlock &p_cb);

    void updateInplacePreview(const VEditor *p_editor,
                              const QTextDocument *p_doc,
                              const QPixmap &p_image,
                              const QString &p_imageName,
                              const QString &p_background);

    void updateInplacePreview(const VEditor *p_editor,
                              const QTextDocument *p_doc,
                              const QPixmap &p_image)
    {
        updateInplacePreview(p_editor, p_doc, p_image, QString(), QString());
    }

    VCodeBlock &codeBlock()
    {
        return m_codeBlock;
    }

    const VCodeBlock &codeBlock() const
    {
        return m_codeBlock;
    }

    bool inplacePreviewReady() const
    {
        return !m_inplacePreview.isNull();
    }

    bool hasImageData() const
    {
        return !m_imgData.isEmpty();
    }

    const QString &imageData() const
    {
        return m_imgData;
    }

    const QString &imageFormat() const
    {
        return m_imgFormat;
    }

    void setImageData(const QString &p_format, const QString &p_data)
    {
        m_imgFormat = p_format;
        m_imgData = p_data;
    }

    const QSharedPointer<VImageToPreview> &inplacePreview() const
    {
        return m_inplacePreview;
    }

private:
    static int getImageIndex()
    {
        static int index = 0;
        return ++index;
    }

    VCodeBlock m_codeBlock;

    QString m_imgData;

    QString m_imgFormat;

    QSharedPointer<VImageToPreview> m_inplacePreview;
};


struct LivePreviewInfo
{
    LivePreviewInfo()
        : m_startPos(),
          m_endPos()
    {
    }

    void clear()
    {
        m_startPos = 0;
        m_endPos = 0;
    }

    void update(const VCodeBlock &p_cb)
    {
        m_startPos = p_cb.m_startPos;
        m_endPos = p_cb.m_startPos + p_cb.m_text.size();
    }

    bool isValid() const
    {
        return m_endPos > m_startPos;
    }

    int m_startPos;
    int m_endPos;
};


// Manage live preview and inplace preview.
class VLivePreviewHelper : public QObject
{
    Q_OBJECT
public:
    VLivePreviewHelper(VEditor *p_editor,
                       VDocument *p_document,
                       QObject *p_parent = nullptr);

    void updateLivePreview();

    void setLivePreviewEnabled(bool p_enabled);

    void setInplacePreviewEnabled(bool p_enabled);

    bool isPreviewEnabled() const;

    const LivePreviewInfo &getLivePreviewInfo() const;

public slots:
    void updateCodeBlocks(TimeStamp p_timeStamp, const QVector<VCodeBlock> &p_codeBlocks);

signals:
    void inplacePreviewCodeBlockUpdated(const QVector<QSharedPointer<VImageToPreview> > &p_images);

    void checkBlocksForObsoletePreview(const QList<int> &p_blocks);

private slots:
    void handleCursorPositionChanged();

    void localAsyncResultReady(int p_id, TimeStamp p_timeStamp, const QString &p_format, const QString &p_result);

    void mathjaxPreviewResultReady(int p_identitifer,
                                   int p_id,
                                   TimeStamp p_timeStamp,
                                   const QString &p_format,
                                   const QByteArray &p_data);

private:
    struct CodeBlockImageCacheEntry
    {
        #define SCALE_FACTOR_THRESHOLD 1.1

        CodeBlockImageCacheEntry()
            : m_ts(0)
        {
        }

        CodeBlockImageCacheEntry(TimeStamp p_ts,
                                 const QString &p_format,
                                 const QByteArray &p_data,
                                 const QString &p_background,
                                 qreal p_scaleFactor)
            : m_ts(p_ts)
        {
            if (!p_data.isEmpty()) {
                m_imageBackground = p_background;

                if (p_scaleFactor < SCALE_FACTOR_THRESHOLD) {
                    m_image.loadFromData(p_data,
                                         p_format.toLocal8Bit().data());
                } else {
                    if (p_format == "svg") {
                        m_image = VUtils::svgToPixmap(p_data,
                                                      m_imageBackground,
                                                      p_scaleFactor);
                    } else {
                        QPixmap tmpImg;
                        tmpImg.loadFromData(p_data,
                                            p_format.toLocal8Bit().data());
                        m_image = tmpImg.scaledToWidth(tmpImg.width() * p_scaleFactor,
                                                       Qt::SmoothTransformation);
                    }
                }
            }
        }

        CodeBlockImageCacheEntry(TimeStamp p_ts,
                                 const QString &p_format,
                                 const QString &p_data,
                                 const QString &p_background,
                                 qreal p_scaleFactor)
            : m_ts(p_ts),
              m_imgData(p_data),
              m_imgFormat(p_format)
        {
            if (!p_data.isEmpty()) {
                m_imageBackground = p_background;

                if (p_scaleFactor < SCALE_FACTOR_THRESHOLD) {
                    m_image.loadFromData(p_data.toUtf8(),
                                         p_format.toLocal8Bit().data());
                } else {
                    if (p_format == "svg") {
                        m_image = VUtils::svgToPixmap(p_data.toUtf8(),
                                                      m_imageBackground,
                                                      p_scaleFactor);
                    } else {
                        QPixmap tmpImg;
                        tmpImg.loadFromData(p_data.toUtf8(),
                                            p_format.toLocal8Bit().data());
                        m_image = tmpImg.scaledToWidth(tmpImg.width() * p_scaleFactor,
                                                       Qt::SmoothTransformation);
                    }
                }
            }
        }

        bool hasImageData() const
        {
            return !m_imgData.isEmpty();
        }

        bool hasImage() const
        {
            return !m_image.isNull();
        }

        TimeStamp m_ts;

        // For live preview.
        QString m_imgData;
        QString m_imgFormat;

        // For in-place preview.
        QPixmap m_image;
        QString m_imageName;
        QString m_imageBackground;
    };

    void checkLang(const QString &p_lang, bool &p_livePreview, bool &p_inplacePreview) const;

    // Get image data for this code block for inplace preview.
    void processForInplacePreview(int p_idx);

    // Emit signal to update inplace preview.
    void updateInplacePreview();

    qreal getScaleFactor(const CodeBlockPreviewInfo &p_cb);

    void clearObsoleteCache();

    void performSmartLivePreview();

    bool isOnlineLivePreview(const QString &p_lang) const;

    // Sorted by m_startBlock in ascending order.
    QVector<CodeBlockPreviewInfo> m_codeBlocks;

    VEditor *m_editor;

    VDocument *m_document;

    QTextDocument *m_doc;

    // Current previewed code block index in m_codeBlocks.
    int m_cbIndex;

    bool m_flowchartEnabled;
    bool m_mermaidEnabled;
    int m_plantUMLMode;
    bool m_graphvizEnabled;
    bool m_mathjaxEnabled;

    bool m_livePreviewEnabled;

    bool m_inplacePreviewEnabled;

    VGraphvizHelper *m_graphvizHelper;
    VPlantUMLHelper *m_plantUMLHelper;

    VMathJaxPreviewHelper *m_mathJaxHelper;

    // Identification for VMathJaxPreviewHelper.
    int m_mathJaxID;

    int m_lastInplacePreviewSize;

    TimeStamp m_timeStamp;

    const qreal m_scaleFactor;

    // Indexed by content.
    QHash<QString, QSharedPointer<CodeBlockImageCacheEntry>> m_cache;

    int m_lastCursorBlock;

    QTimer *m_livePreviewTimer;

    LivePreviewInfo m_curLivePreviewInfo;
};

inline bool VLivePreviewHelper::isPreviewEnabled() const
{
    return m_inplacePreviewEnabled || m_livePreviewEnabled;
}

inline qreal VLivePreviewHelper::getScaleFactor(const CodeBlockPreviewInfo &p_cb)
{
    if (p_cb.codeBlock().m_lang == QStringLiteral("mathjax")) {
        return 1.0;
    } else {
        return m_scaleFactor;
    }
}

inline const LivePreviewInfo &VLivePreviewHelper::getLivePreviewInfo() const
{
    return m_curLivePreviewInfo;
}
#endif // VLIVEPREVIEWHELPER_H
