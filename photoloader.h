#ifndef PHOTOLOADER_H
#define PHOTOLOADER_H

#include <QObject>
#include <QFileInfo>
#include <QImage>

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

private:

    QFileInfo m_FileInfo;
    QImage::Format m_ImageFormat;
    QStringList m_FormatList;
    uint m_xSize;
    uint m_ySize;
    bool m_autoScale = true;

signals:

};

#endif // PHOTOLOADER_H
