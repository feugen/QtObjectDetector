#include "photoloader.h"

PhotoLoader::PhotoLoader(QObject *parent) : MediaLoader(parent),
    m_ImageFormat(QImage::Format_BGR888)
  , m_xSize(800)
  , m_ySize(600)
{
}


QFileInfo PhotoLoader::getFileInfo() const
{
    return m_FileInfo;
}

void PhotoLoader::setFileInfo(const QFileInfo &FileInfo)
{
    m_FileInfo = FileInfo;
}

QImage::Format PhotoLoader::getImageFormat() const
{
    return m_ImageFormat;
}

void PhotoLoader::setImageFormat(const QImage::Format &ImageFormat)
{
    m_ImageFormat = ImageFormat;
}

uint PhotoLoader::getXSize() const
{
    return m_xSize;
}

void PhotoLoader::setXSize(const uint &xSize)
{
    m_xSize = xSize;
}

uint PhotoLoader::getYSize() const
{
    return m_ySize;
}

void PhotoLoader::setYSize(const uint &ySize)
{
    m_ySize = ySize;
}

bool PhotoLoader::getAutoScale() const
{
    return m_autoScale;
}

void PhotoLoader::setAutoScale(bool autoScale)
{
    m_autoScale = autoScale;
}

bool PhotoLoader::getAutoSize() const
{
    return m_autoSize;
}

void PhotoLoader::setAutoSize(bool autoSize)
{
    m_autoSize = autoSize;
}
