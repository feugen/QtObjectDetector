#ifndef PHOTOLOADER_H
#define PHOTOLOADER_H

#include <QObject>
#include <QFileInfo>
#include <QImage>

class PhotoLoader : public QObject
{
    Q_OBJECT

public:

    enum e_ColorFormat
    {
        COLOR = QImage::Format_BGR888, //Todo - Only 2 Formats are supported
        GRAY = QImage::Format_Indexed8 //Todo - this way we can cast easily and dont need conversion right now (temp. solution)
    };
    Q_ENUM(e_ColorFormat)

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
