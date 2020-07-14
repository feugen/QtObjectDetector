#ifndef VIDEOLOADER_H
#define VIDEOLOADER_H

#include <QObject>
#include <QCameraInfo>
#include "medialoader.h"

class VideoLoader : public MediaLoader
{
    Q_OBJECT
public:
    explicit VideoLoader(QObject *parent = nullptr);

    QFileInfo getFileInfo() const override;
    void setFileInfo(const QFileInfo &FileInfo) override;

    QCameraInfo CameraInfo() const;

private:
    QCameraInfo m_CameraInfo;

signals:

};

#endif // VIDEOLOADER_H
