#ifndef BASE_H
#define BASE_H

#include <QObject>
#include <QImage>
#include <QMetaEnum>
#include <opencv2/opencv.hpp>

class Base : public QObject
{
    Q_OBJECT
public:
    explicit Base(QObject *parent = nullptr);

    template<typename QEnum>
    static QString QEnumToQString (const QEnum value)
    {
      return QMetaEnum::fromType<QEnum>().valueToKey(value);
    }

    enum e_OpenCVColorFormat
    {
        COLOR = QImage::Format_BGR888,
        GRAY = QImage::Format_Indexed8
    };
    Q_ENUM(e_OpenCVColorFormat)

    enum e_OpenCVThresholdType
    {
        THRESH_BINARY = cv::THRESH_BINARY,
        THRESH_BINARY_INV = cv::THRESH_BINARY_INV,
        THRESH_TRUNC = cv::THRESH_TRUNC,
        THRESH_TOZERO = cv::THRESH_TOZERO,
        THRESH_TOZERO_INV = cv::THRESH_TOZERO_INV
    };
    Q_ENUM(e_OpenCVThresholdType)

    enum e_OpenCVAdaptivThresholdMethod
    {
        THRESH_MEAN_C = cv::ADAPTIVE_THRESH_MEAN_C,
        THRESH_GAUSSIAN_C = cv::ADAPTIVE_THRESH_GAUSSIAN_C
    };
    Q_ENUM(e_OpenCVAdaptivThresholdMethod)

    enum e_OpenCVAdaptiveThresholdType
    {
        THRESH_ADAPTIV_BINARY = cv::THRESH_BINARY,
        THRESH_ADAPTIV_BINARY_INV = cv::THRESH_BINARY_INV,
    };
    Q_ENUM(e_OpenCVAdaptiveThresholdType)

    enum e_OpenCVAdaptiveThresholdBlocksize
    {
        BLOCKSIZE_3 = 3,
        BLOCKSIZE_5 = 5,
        BLOCKSIZE_7 = 7,
        BLOCKSIZE_9 = 9,
        BLOCKSIZE_11 = 11
    };
    Q_ENUM(e_OpenCVAdaptiveThresholdBlocksize)

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
