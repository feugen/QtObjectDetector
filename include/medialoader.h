#ifndef MEDIALOADER_H
#define MEDIALOADER_H

#include <QObject>
#include <QFileInfo>
#include <QImage>
#include <QMetaEnum>

class MediaLoader : public QObject
{
    Q_OBJECT

public:
    virtual QFileInfo getFileInfo() const = 0;
    virtual void setFileInfo(const QFileInfo &FileInfo) = 0;

    QStringList getFormatList() const;
    void setFormatList(const QStringList &FormatList);

protected:
    explicit MediaLoader(QObject *parent = nullptr);


protected:
    QFileInfo m_FileInfo;
    QStringList m_FormatList;

signals:

};

#endif // MEDIALOADER_H
