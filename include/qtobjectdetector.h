#ifndef OBJECTDETECTOR_H
#define OBJECTDETECTOR_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QCamera>
#include <QGraphicsScene>
#include <memory>
#include <functional>
#include "photoloader.h"
#include "videoloader.h"
#include "pipelinehandler.h"
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class QtObjectDetector; }
QT_END_NAMESPACE

class QtObjectDetector : public QMainWindow
{
    Q_OBJECT

public:
    QtObjectDetector(QWidget *parent = nullptr);
    ~QtObjectDetector();

private slots:

    void on_pushButton_ApplyFunction_clicked();

    //Image part
    void on_selectFileButton_clicked();
    void on_loadFilePushButton_clicked();
    void on_fileSelected(const QString &file);
    void on_checkBox_autoSize_toggled(bool checked);
    void on_pushButton_AddToPipeline_clicked();
    void on_pushButton_DeleteFromPipeline_clicked();
    void on_pushButton_SavePipeline_clicked();
    void on_pushButton_ApplyPipeline_clicked();

    void applyCvtColor(Base::e_OpenCVColorFormat selectedColorFormat);
    void applyThreshold(double threshold, double thresholdMax, int type);
    void applyAdaptiveThreshold(double maxValue, int adaptiveMethod, int thresholdType, int blockSize, double C);

    //Video Part
    void on_videoSelected(const QString &file);
    void on_pushButton_StartCamera_clicked();
    void on_checkBox_LiveCamera_stateChanged(int arg1);
    void on_pushButton_SelectVideo_clicked();
    void on_pushButton_LoadVideo_clicked();
    void on_checkBox_AutoResolution_stateChanged(int arg1);

    void on_pushButton_DeletePipeline_clicked();

    void on_lineEdit_PipelineName_textChanged(const QString &arg1);

    void on_comboBox_FunctionSelector_currentIndexChanged(const QString &arg1);

private:
    void storeImageSettings();
    void loadImage();
    void loadImageToQLabel(const size_t &storagePosition = 0);
    void stopCamera() const;

private:
    Ui::QtObjectDetector *ui;
    std::unique_ptr<PipelineHandler> m_pPipelineHandler = nullptr;
    //Image part
    std::unique_ptr<PhotoLoader> m_pPhotoLoader = nullptr;
    std::unique_ptr<QFileDialog> m_pImageDialog = nullptr;
    std::function<void()> m_lastFunction;
    QString m_lastFunctionString;
    cv::Mat m_originalImage;
    //Video Part
    std::unique_ptr<VideoLoader> m_pVideoLoader = nullptr;
    std::unique_ptr<QFileDialog> m_pVideoDialog = nullptr;
    std::unique_ptr<QCamera> m_pCameraActive = nullptr;
    std::unique_ptr<cv::Mat> m_pOutputVideoImage = nullptr;
    std::unique_ptr<cv::VideoCapture> m_pInputVideo = nullptr;
    QList<QCameraInfo> m_cameras;
    Base::ImageSettings m_imageSettings;
};
#endif // OBJECTDETECTOR_H
