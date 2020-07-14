#include "videoloader.h"

VideoLoader::VideoLoader(QObject *parent) : MediaLoader(parent)
{

}

QFileInfo VideoLoader::getFileInfo() const
{
    return m_FileInfo;
}

void VideoLoader::setFileInfo(const QFileInfo &FileInfo)
{
    m_FileInfo = FileInfo;
}

QCameraInfo VideoLoader::CameraInfo() const
{
    return m_CameraInfo;
}
