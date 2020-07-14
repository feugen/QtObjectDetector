#ifndef PHOTOLOADER_H
#define PHOTOLOADER_H

#include <QObject>
#include "base.h"
#include "medialoader.h"

class PhotoLoader : public MediaLoader
{
    Q_OBJECT

public:

    explicit PhotoLoader(QObject *parent = nullptr);

    QFileInfo getFileInfo() const override;
    void setFileInfo(const QFileInfo &FileInfo) override;

    QImage::Format getImageFormat() const;
    void setImageFormat(const QImage::Format &ImageFormat);
    uint getXSize() const;
    void setXSize(const uint &xSize);
    uint getYSize() const;
    void setYSize(const uint &ySize);
    bool getAutoScale() const;
    void setAutoScale(bool autoScale);
    bool getAutoSize() const;
    void setAutoSize(bool autoSize);

private:
    QImage::Format m_ImageFormat;
    uint m_xSize;
    uint m_ySize;
    bool m_autoSize = true;
    bool m_autoScale = false;

signals:

};

#endif // PHOTOLOADER_H
