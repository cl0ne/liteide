#include "imageeditorfactory.h"
#include "mimetype/mimetype.h"
#include "imageeditor.h"
#include <QImageReader>
#if QT_VERSION >= 0x050000
#include <QMimeDatabase>
#include <QMimeType>
#endif
#include <QDebug>

ImageEditorFactory::ImageEditorFactory(IApplication *app, QObject *parent)
    : LiteApi::IEditorFactory(parent), m_liteApp(app)
{
#if QT_VERSION >= 0x050000
    QList<QByteArray> supportFormats = QImageReader::supportedImageFormats();
    QMimeDatabase db;
    foreach (QByteArray _type, QImageReader::supportedMimeTypes()) {
        QString type = QString::fromUtf8(_type);
        QMimeType mt = db.mimeTypeForName(type);
        if (!mt.isValid()) {
            continue;
        }
        QStringList patterns;
        foreach(QString fmt, mt.suffixes()) {
            if (supportFormats.contains(fmt.toUtf8())) {
                patterns << "*."+fmt;
            }
        }
        if (patterns.isEmpty()) {
            continue;
        }
        MimeType *mimeType = new MimeType;
        mimeType->setType(type);
        mimeType->setComment(mt.comment());
        foreach (QString p, patterns) {
            mimeType->appendGlobPatterns(p);
        }
        m_liteApp->mimeTypeManager()->addMimeType(mimeType);
        m_mimeTypes.append(type);
    }
#else
    QString mimeTypeList = "image/bmp:bmp;image/gif:gif;image/jpeg:jpg,jpeg;image/png:png;image/x-portable-bitmap:pbm;image/x-portable-graymap:pgm;image/x-portable-pixmap:ppm;image/x-xbitmap:xbm;image/x-xpixmap:xpm;image/svg+xml:svg;";
    QList<QByteArray> supportFormats = QImageReader::supportedImageFormats();
    foreach (QString mtype, mimeTypeList.split(";",QString::SkipEmptyParts)) {
        QStringList ar = mtype.split(":",QString::SkipEmptyParts);
        if (ar.size() != 2) {
            continue;
        }
        QString type = ar[0];
        QStringList fmts = ar[1].split(",",QString::SkipEmptyParts);
        QStringList patterns;
        foreach (QString fmt, fmts) {
            if (supportFormats.contains(fmt.toUtf8())) {
                patterns << "*."+fmt;
            }
        }
        if (patterns.isEmpty()) {
            continue;
        }
        MimeType *mimeType = new MimeType;
        mimeType->setType(type);
        mimeType->setComment(QString("%1 Image").arg(type));
        foreach (QString p, patterns) {
            mimeType->appendGlobPatterns(p);
        }
        m_liteApp->mimeTypeManager()->addMimeType(mimeType);
        m_mimeTypes.append(type);
    }
#endif
}

QStringList ImageEditorFactory::mimeTypes() const
{
    return  m_mimeTypes;
}

IEditor *ImageEditorFactory::open(const QString &fileName, const QString &mimeType)
{
    if (!m_mimeTypes.contains(mimeType)) {
        return  0;
    }
    ImageEditor *view = new ImageEditor(m_liteApp);
    if (!view->open(fileName,mimeType)) {
        delete  view;
        return  0;
    }
    return  view;
}

IEditor *ImageEditorFactory::create(const QString &contents, const QString &mimeType)
{
    return  0;
}
