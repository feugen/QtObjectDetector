#include "medialoader.h"

MediaLoader::MediaLoader(QObject *parent) : QObject(parent),
    m_FileInfo(QFileInfo("No File selected"))
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

QStringList MediaLoader::getFormatList() const
{
    return m_FormatList;
}

void MediaLoader::setFormatList(const QStringList &FormatList)
{
    m_FormatList = FormatList;
}
