#ifndef PIPELINEHANDLER_H
#define PIPELINEHANDLER_H

#include <QObject>
#include "base.h"
#include <opencv2/opencv.hpp>

class PipelineHandler : public QObject
{
    Q_OBJECT
public:
    explicit PipelineHandler(QObject *parent = nullptr);

    std::function<void(cv::InputArray, cv::OutputArray, int, int)> m_cvtColor = cv::cvtColor;

    std::vector<std::pair<cv::Mat, Base::e_ColorFormat>> &getImagePipeline();
    std::vector<std::function<void()>> &getFunctionPipeline();
    std::vector<std::pair<std::vector<std::function<void()>>, QString>> &getAvailablePipelines();

private:
    std::vector<std::pair<cv::Mat, Base::e_ColorFormat>> m_imagePipeline;
    std::vector<std::function<void()>> m_functionPipeline;
    std::vector<std::pair<std::vector<std::function<void()>>, QString>> m_availablePipelines;

signals:

};

#endif // PIPELINEHANDLER_H
