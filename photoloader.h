#ifndef PHOTOLOADER_H
#define PHOTOLOADER_H

#include <QObject>
#include <QDir>
#include <QImage>

class PhotoLoader : public QObject
{
    Q_OBJECT
public:
    explicit PhotoLoader(QObject *parent = nullptr);

private:
    QDir m_Path;
    QImage::Format m_ImageFormat;
    uint m_xSize;
    uint m_ySize;

signals:

};

#endif // PHOTOLOADER_H
