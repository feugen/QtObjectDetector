#ifndef PIPELINEHANDLER_H
#define PIPELINEHANDLER_H

#include <QObject>
#include "base.h"

class PipelineHandler : public QObject
{
    Q_OBJECT
public:
    explicit PipelineHandler(QObject *parent = nullptr);

    std::vector<std::pair<cv::Mat, Base::e_OpenCVColorFormat>> &getImagePipeline();
    std::vector<std::function<void()>> &getFunctionPipeline();
    std::vector<std::pair<std::vector<std::function<void()>>, QString>> &getAvailablePipelines();

    std::function<void (cv::InputArray, cv::OutputArray, int, int)> &getCvtColor();
    std::function<void (cv::InputArray, cv::OutputArray, double, double, int)> &getThreshold();
    std::function<void (cv::InputArray, cv::OutputArray, double, int, int, int, double)> &getAdaptiveThreshold();

    std::function<void (cv::InputArray, cv::OutputArray, int)> &getMedianBlur();
    std::function<void (cv::InputArray, cv::OutputArray, cv::Size, cv::Point, int)> &getBlur();
    std::function<void (cv::InputArray, cv::OutputArray, cv::Size, double, double, int)> &getGaussianBlur();
    std::function<void (cv::InputArray, cv::OutputArray, cv::InputArray)> &getBitwiseNot();
    std::function<void (cv::InputArray, double, cv::OutputArray)> &getPow();
    std::function<void (cv::InputArray, cv::OutputArray, cv::InputArray, cv::Point, int, int, const cv::Scalar &)> &getErode();
    std::function<void (cv::InputArray, cv::OutputArray, int, int, int, int, double, double, int)> &getSobel();
    std::function<void (cv::InputArray, cv::OutputArray, int, int, double, double, int)> &getLaplacian();
    std::function<void (cv::InputArray, double, cv::InputArray, double, double, cv::OutputArray, int)> &getAddWeighted();

private:
    std::function<void(cv::InputArray, cv::OutputArray, int, int)> m_cvtColor = cv::cvtColor;
    std::function<void(cv::InputArray, cv::OutputArray, double, double, int)> m_threshold = cv::threshold;
    std::function<void(cv::InputArray, cv::OutputArray, double, int, int, int, double)> m_adaptiveThreshold = cv::adaptiveThreshold;

    std::function<void(cv::InputArray, cv::OutputArray, int)> m_medianBlur = cv::medianBlur;
    std::function<void(cv::InputArray, cv::OutputArray, cv::Size, cv::Point, int)> m_blur = cv::blur;
    std::function<void(cv::InputArray, cv::OutputArray, cv::Size, double, double, int)> m_gaussianBlur = cv::GaussianBlur;
    std::function<void(cv::InputArray, cv::OutputArray, cv::InputArray)> m_bitwiseNot = cv::bitwise_not;
    std::function<void(cv::InputArray, double, cv::OutputArray)> m_pow = static_cast<void(*)(cv::InputArray, double, cv::OutputArray)>(cv::pow);
    std::function<void(cv::InputArray, cv::OutputArray, cv::InputArray, cv::Point, int, int, const cv::Scalar&)> m_erode = cv::erode;
    std::function<void(cv::InputArray, cv::OutputArray, int, int, int, int, double, double, int)> m_sobel = cv::Sobel;
    std::function<void(cv::InputArray, cv::OutputArray, int, int, double, double, int)> m_laplacian = cv::Laplacian;
    std::function<void(cv::InputArray, double, cv::InputArray, double, double, cv::OutputArray, int)> m_addWeighted = cv::addWeighted;

private:
    std::vector<std::pair<cv::Mat, Base::e_OpenCVColorFormat>> m_imagePipeline;
    std::vector<std::function<void()>> m_functionPipeline;
    std::vector<std::pair<std::vector<std::function<void()>>, QString>> m_availablePipelines;

signals:

};

#endif // PIPELINEHANDLER_H
