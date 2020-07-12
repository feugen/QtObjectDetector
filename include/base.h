#ifndef BASE_H
#define BASE_H

#include <QObject>
#include <QImage>

class Base : public QObject
{
    Q_OBJECT
public:
    explicit Base(QObject *parent = nullptr);

    //Only 2 Formats are supported
    enum e_OpenCVColorFormat
    {
        COLOR = QImage::Format_BGR888,
        GRAY = QImage::Format_Indexed8
    };
    Q_ENUM(e_OpenCVColorFormat)

    enum e_OpenCVThresholdType
    {
        THRESH_BINARY = 0,
        THRESH_BINARY_INV = 1,
        THRESH_TRUNC = 2,
        THRESH_TOZERO = 3,
        THRESH_TOZERO_INV = 4
    };
    Q_ENUM(e_OpenCVThresholdType)

    enum e_OpenCVFunction
    {
        cvtColor = 0,
        threshold = 1,
        adaptiveThreshold = 2
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
