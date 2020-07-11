#ifndef PHOTOLOADER_H
#define PHOTOLOADER_H

#include <QObject>
#include <QFileInfo>
#include "base.h"

class PhotoLoader : public QObject
{
    Q_OBJECT

public:

    explicit PhotoLoader(QObject *parent = nullptr);

    QFileInfo getFileInfo() const;
    void setFileInfo(const QFileInfo &Path);
    QImage::Format getImageFormat() const;
    void setImageFormat(const QImage::Format &ImageFormat);
    QStringList getFormatList() const;
    void setFormatList(const QStringList &FormatList);
    uint getXSize() const;
    void setXSize(const uint &xSize);
    uint getYSize() const;
    void setYSize(const uint &ySize);
    bool getAutoScale() const;
    void setAutoScale(bool autoScale);
    bool getAutoSize() const;
    void setAutoSize(bool autoSize);

private:

    QStringList m_FormatList;
    QFileInfo m_FileInfo;
    QImage::Format m_ImageFormat;
    uint m_xSize;
    uint m_ySize;
    bool m_autoSize = true;
    bool m_autoScale = false;

signals:

};

#endif // PHOTOLOADER_H
