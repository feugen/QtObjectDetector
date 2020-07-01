#ifndef VIDEOLOADER_H
#define VIDEOLOADER_H

#include <QObject>
#include <QFileInfo>
#include <QCameraInfo>

class VideoLoader : public QObject
{
    Q_OBJECT
public:
    explicit VideoLoader(QObject *parent = nullptr);

    QFileInfo getFileInfo() const;
    void setFileInfo(const QFileInfo &FileInfo);

    QCameraInfo CameraInfo() const;

private:
    QFileInfo m_FileInfo;
    QCameraInfo m_CameraInfo;

signals:

};

#endif // VIDEOLOADER_H
