#include "photoloader.h"
#include <QMetaEnum>

PhotoLoader::PhotoLoader(QObject *parent) : QObject(parent),
    m_FileInfo(QFileInfo("No File selected"))
  , m_ImageFormat(QImage::Format_BGR888)
  , m_xSize(800)
  , m_ySize(600)
{
    const QMetaEnum formatEnum = QMetaEnum::fromType<QImage::Format>();

    for(int i = 0; i < formatEnum.keyCount(); i++){
        QString item = formatEnum.valueToKey(i);
        if(item == "Format_Indexed8" || item == "Format_BGR888") //Only 2 Formats are supported for now
        {
            m_FormatList += item.remove("Format_");
        }
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

bool PhotoLoader::getAutoSize() const
{
    return m_autoSize;
}

void PhotoLoader::setAutoSize(bool autoSize)
{
    m_autoSize = autoSize;
}
