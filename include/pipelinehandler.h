#ifndef PIPELINEHANDLER_H
#define PIPELINEHANDLER_H

#include <QObject>
#include <opencv2/opencv.hpp>

class PipelineHandler : public QObject
{
    Q_OBJECT
public:
    explicit PipelineHandler(QObject *parent = nullptr);

    std::function<void(cv::InputArray, cv::OutputArray, int, int)> m_cvtColor = cv::cvtColor;

    enum e_OpenCVFunction {
        cvtColor,
    };
    Q_ENUM(e_OpenCVFunction)

private:

signals:

};

#endif // PIPELINEHANDLER_H
