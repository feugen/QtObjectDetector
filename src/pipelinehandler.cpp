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

std::function<void (cv::InputArray, cv::OutputArray, double, double, int)> &PipelineHandler::getThreshold()
{
    return m_threshold;
}

std::function<void (cv::InputArray, cv::OutputArray, double, int, int, int, double C)> &PipelineHandler::getAdaptiveThreshold()
{
    return m_adaptiveThreshold;
}

std::function<void (cv::InputArray, cv::OutputArray, int)> &PipelineHandler::getMedianBlur()
{
    return m_medianBlur;
}

std::function<void (cv::InputArray, cv::OutputArray, cv::Size, cv::Point, int)> &PipelineHandler::getBlur()
{
    return m_blur;
}

std::function<void (cv::InputArray, cv::OutputArray, cv::Size, double, double, int)> &PipelineHandler::getGaussianBlur()
{
    return m_gaussianBlur;
}

std::function<void (cv::InputArray, cv::OutputArray, cv::InputArray)> &PipelineHandler::getBitwiseNot()
{
    return m_bitwiseNot;
}

std::function<void (cv::InputArray, double, cv::OutputArray)> &PipelineHandler::getPow()
{
    return m_pow;
}

std::function<void (cv::InputArray, cv::OutputArray, cv::InputArray, cv::Point, int, int, const cv::Scalar &)> &PipelineHandler::getErode()
{
    return m_erode;
}

std::function<void (cv::InputArray, cv::OutputArray, int, int, int, int, double, double, int)> &PipelineHandler::getSobel()
{
    return m_sobel;
}

std::function<void (cv::InputArray, cv::OutputArray, int, int, double, double, int)> &PipelineHandler::getLaplacian()
{
     return m_laplacian;
}

std::function<void (cv::InputArray, cv::OutputArray, double, double, int, bool)> &PipelineHandler::getCanny()
{
     return m_canny;
}

std::function<void (cv::InputArray, double, cv::InputArray, double, double, cv::OutputArray, int)> &PipelineHandler::getAddWeighted()
{
    return m_addWeighted;
}

std::function<void (cv::InputArray, cv::OutputArray, cv::CascadeClassifier)> &PipelineHandler::getCascadeClassifier()
{
    m_cascadeClassifier = [](cv::InputArray input, cv::OutputArray output, cv::CascadeClassifier cascade) -> void
    {
        cv::Mat tempGray;
        const int channels = input.channels();
        if(channels == 3)
        {
            cv::cvtColor(input, tempGray, cv::COLOR_BGR2GRAY );
        }
        else if(channels == 1)
        {
            input.copyTo(tempGray);
        }

        std::vector<cv::Rect> cascadeResults;
        cascade.detectMultiScale(tempGray, cascadeResults);
        cv::Scalar color(0,255,0,255);

        for (const cv::Rect &result : cascadeResults)
        {
            cv::rectangle(input.getMat(), result, color, 2);
        }

        input.copyTo(output);
    };

return m_cascadeClassifier;
}

std::function<void (cv::InputArray, cv::OutputArray, int, double, double, cv::InputArray, int, bool, double)> &PipelineHandler::getShiTomasi()
{
    return m_shiTomasi;
}
