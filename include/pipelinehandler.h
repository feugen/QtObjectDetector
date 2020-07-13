#ifndef PIPELINEHANDLER_H
#define PIPELINEHANDLER_H

#include <QObject>
#include "base.h"

class PipelineHandler : public QObject
{
    Q_OBJECT
public:
    explicit PipelineHandler(QObject *parent = nullptr);

    std::function<void(cv::InputArray, cv::OutputArray, int, int)> m_cvtColor = cv::cvtColor;
    std::function<void(cv::InputArray, cv::OutputArray, double threshold, double max_value, int threshold_type)> m_threshold = cv::threshold;
    std::function<void(cv::InputArray, cv::OutputArray, double maxValue, int adaptiveMethod, int thresholdType, int blockSize, double C)> m_adaptiveThreshold = cv::adaptiveThreshold;

    std::vector<std::pair<cv::Mat, Base::e_OpenCVColorFormat>> &getImagePipeline();
    std::vector<std::function<void()>> &getFunctionPipeline();
    std::vector<std::pair<std::vector<std::function<void()>>, QString>> &getAvailablePipelines();

private:
    std::vector<std::pair<cv::Mat, Base::e_OpenCVColorFormat>> m_imagePipeline;
    std::vector<std::function<void()>> m_functionPipeline;
    std::vector<std::pair<std::vector<std::function<void()>>, QString>> m_availablePipelines;

signals:

};

#endif // PIPELINEHANDLER_H
