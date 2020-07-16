#include "pipelinehandler.h"

PipelineHandler::PipelineHandler(QObject *parent) : QObject(parent)
{

}

std::vector<std::pair<cv::Mat, Base::e_OpenCVColorFormat>> &PipelineHandler::getImagePipeline()
{
    return m_imagePipeline;
}

std::vector<std::function<void()>> &PipelineHandler::getFunctionPipeline()
{
    return m_functionPipeline;
}

std::vector<std::pair<std::vector<std::function<void()>>, QString>> &PipelineHandler::getAvailablePipelines()
{
    return m_availablePipelines;
                                                                    }
                                                                    
std::function<void (cv::InputArray, cv::OutputArray, int, int)> &PipelineHandler::getCvtColor()
{
    return m_cvtColor;
                                                                    }

std::function<void (cv::InputArray, cv::OutputArray, double threshold, double max_value, int threshold_type)> &PipelineHandler::getThreshold()
{
 return m_threshold;
                                                                    }

std::function<void (cv::InputArray, cv::OutputArray, double maxValue, int adaptiveMethod, int thresholdType, int blockSize, double C)> &PipelineHandler::getAdaptiveThreshold()
{
 return m_adaptiveThreshold;
}
                                                                    
