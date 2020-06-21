#include "photoloader.h"
#include <QMetaEnum>

PhotoLoader::PhotoLoader(QObject *parent) : QObject(parent),
    m_FileInfo(QFileInfo("No File selected")),
    m_ImageFormat(QImage::Format_BGR888),
    m_xSize(800),
    m_ySize(600)
{
    QMetaEnum formatEnum = QMetaEnum::fromType<QImage::Format>();

    for(int i = 0; i < formatEnum.keyCount(); i++){
        m_FormatList += formatEnum.valueToKey(i);
    }
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

QStringList PhotoLoader::getFormatList() const
{
    return m_FormatList;
}

void PhotoLoader::setFormatList(const QStringList &FormatList)
{
    m_FormatList = FormatList;
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
