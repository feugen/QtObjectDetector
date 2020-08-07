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

    template<typename QEnum>
    static QString QEnumToQString (const QEnum value)
    {
      return QMetaEnum::fromType<QEnum>().valueToKey(static_cast<int>(value));
    }

    enum class e_OpenCVColorFormat
    {
        COLOR = QImage::Format_BGR888,
        COLOR_INV = QImage::Format_RGB888,
        GRAY = QImage::Format_Indexed8,
        UNKNOWN = 1000
    };
    Q_ENUM(e_OpenCVColorFormat)

    enum class e_OpenCVThresholdType
    {
        THRESH_BINARY = cv::THRESH_BINARY,
        THRESH_BINARY_INV = cv::THRESH_BINARY_INV,
        THRESH_TRUNC = cv::THRESH_TRUNC,
        THRESH_TOZERO = cv::THRESH_TOZERO,
        THRESH_TOZERO_INV = cv::THRESH_TOZERO_INV
    };
    Q_ENUM(e_OpenCVThresholdType)

    enum class e_OpenCVAdaptivThresholdMethod
    {
        THRESH_MEAN_C = cv::ADAPTIVE_THRESH_MEAN_C,
        THRESH_GAUSSIAN_C = cv::ADAPTIVE_THRESH_GAUSSIAN_C
    };
    Q_ENUM(e_OpenCVAdaptivThresholdMethod)

    enum class e_OpenCVAdaptiveThresholdType
    {
        THRESH_ADAPTIV_BINARY = cv::THRESH_BINARY,
        THRESH_ADAPTIV_BINARY_INV = cv::THRESH_BINARY_INV,
    };
    Q_ENUM(e_OpenCVAdaptiveThresholdType)

    enum class e_OpenCVBlockSize
    {
        BLOCKSIZE_3 = 3,
        BLOCKSIZE_5 = 5,
        BLOCKSIZE_7 = 7,
        BLOCKSIZE_9 = 9,
        BLOCKSIZE_11 = 11
    };
    Q_ENUM(e_OpenCVBlockSize)

    enum class e_OpenCVKSize
    {
        KSIZE_3 = 3,
        KSIZE_5 = 5,
        KSIZE_7 = 7,
        KSIZE_9 = 9,
        KSIZE_11 = 11
    };
    Q_ENUM(e_OpenCVKSize)

    enum class e_OpenCVKSizeExt
    {
        KSIZE_1 = 1,
        KSIZE_3 = 3,
        KSIZE_5 = 5,
        KSIZE_7 = 7
    };
    Q_ENUM(e_OpenCVKSizeExt)

    enum class e_OpenCVErosionSize
    {
        ErosionSize_1 = 1,
        ErosionSize_2 = 2,
        ErosionSize_3 = 3,
        ErosionSize_4 = 4,
        ErosionSize_5 = 5
    };
    Q_ENUM(e_OpenCVErosionSize)

    enum class e_OPenCVBorderType
    {
        BORDER_CONSTANT = cv::BorderTypes::BORDER_CONSTANT,
        BORDER_REPLICATE = cv::BorderTypes::BORDER_REPLICATE,
        BORDER_REFLECT = cv::BorderTypes::BORDER_REFLECT,
        BORDER_WRAP = cv::BorderTypes::BORDER_WRAP,
        BORDER_REFLECT_101 = cv::BorderTypes::BORDER_REFLECT_101,
        BORDER_TRANSPARENT = cv::BorderTypes::BORDER_TRANSPARENT
    };
    Q_ENUM(e_OPenCVBorderType)

    enum class e_OPenCVMorphShapes
    {
        MORPH_RECT = cv::MorphShapes::MORPH_RECT,
        MORPH_CROSS = cv::MorphShapes::MORPH_CROSS,
        MORPH_ELLIPSE = cv::MorphShapes::MORPH_ELLIPSE
    };
    Q_ENUM(e_OPenCVMorphShapes)

    enum class e_OPenCVFunctionType
    {
        TRANSFORMATION = 0,
        FILTERING = 1,
        DETECTION = 2,
        TRACKING = 3
    };
    Q_ENUM(e_OPenCVFunctionType)

    enum class e_OpenCVFunction
    {
        CvtColor = 0,
        Threshold = 1,
        AdaptiveThreshold = 2,
        MedianBlur = 3,
        Blur = 4,
        GaussianBlur = 5,
        BitwiseNot = 6,
        Pow = 7,
        Erode = 8,
        Sobel = 9,
        Laplacian = 10,
        Canny = 11,
        AddWeighted = 12,
        CascadeClassifier = 13,
        ShiTomasi = 14,
        MeanShift = 15,
        CamShift = 16,
        BackgroundSubtraction = 17,
        OpticalFlow = 18
    };
    Q_ENUM(e_OpenCVFunction)

    enum class e_OpenCVFilteringFunction
    {
        MedianBlur,
        Blur,
        GaussianBlur,
        Pow,
        Erode
    };
    Q_ENUM(e_OpenCVFilteringFunction)

    enum class e_OpenCVTransformationFunction
    {
        CvtColor,
        Threshold,
        AdaptiveThreshold,
        BitwiseNot,
        AddWeighted
    };
    Q_ENUM(e_OpenCVTransformationFunction)

    enum class e_OpenCVDetectionFunction
    {
        CascadeClassifier,
        ShiTomasi,
        Sobel,
        Laplacian,
        Canny
    };
    Q_ENUM(e_OpenCVDetectionFunction)

    enum class e_OpenCVTrackingFunction
    {
        MeanShift,
        CamShift,
        BackgroundSubtraction,
        OpticalFlow
    };
    Q_ENUM(e_OpenCVTrackingFunction)

    struct ImageSettings {
        bool autoSize;
        bool autoScaled;
        int x;
        int y;
        QImage::Format imageFormat;
        QByteArray filePath;
    };

signals:

private:
    explicit Base(QObject *parent = nullptr)
    {
        Q_UNUSED(parent)
    }

};

#endif // BASE_H
