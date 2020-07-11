#ifndef BASE_H
#define BASE_H

#include <QObject>
#include <QImage>

class Base : public QObject
{
    Q_OBJECT
public:
    explicit Base(QObject *parent = nullptr);

    enum e_ColorFormat
    {
        COLOR = QImage::Format_BGR888, //Todo - Only 2 Formats are supported
        GRAY = QImage::Format_Indexed8 //Todo - this way we can cast easily and dont need conversion right now (temp. solution)
    };
    Q_ENUM(e_ColorFormat)

    enum e_OpenCVFunction
    {
        cvtColor,
    };
    Q_ENUM(e_OpenCVFunction)

    struct ImageSettings {
        bool autoSize;
        bool autoScaled;
        int x;
        int y;
        QImage::Format imageFormat;
        QByteArray filePath;
    };

signals:

};

#endif // BASE_H
