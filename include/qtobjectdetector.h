#ifndef OBJECTDETECTOR_H
#define OBJECTDETECTOR_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QCamera>
#include <QGraphicsScene>
#include <memory>
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
    void on_autoSizeCheckBox_toggled(bool checked);

    //Video Part
    void on_videoSelected(const QString &file);
    void on_pushButton_StartCamera_clicked();
    void on_checkBox_LiveCamera_stateChanged(int arg1);
    void on_pushButton_SelectVideo_clicked();
    void on_pushButton_LoadVideo_clicked();
    void on_checkBox_AutoResolution_stateChanged(int arg1);

private:
    void loadImage();
    void stopCamera() const;

private:
    Ui::QtObjectDetector *ui;
    std::unique_ptr<PipelineHandler> m_pPipelineHandler = nullptr;
    //Image part
    QGraphicsScene *m_pScene;
    std::unique_ptr<PhotoLoader> m_pPhotoLoader = nullptr;
    std::unique_ptr<QFileDialog> m_pImageDialog = nullptr;
    cv::Mat m_inputImage;
    //Video Part
    std::unique_ptr<VideoLoader> m_pVideoLoader = nullptr;
    std::unique_ptr<QFileDialog> m_pVideoDialog = nullptr;
    std::unique_ptr<QCamera> m_pCameraActive = nullptr;
    std::unique_ptr<cv::Mat> m_pOutputVideoImage = nullptr;
    std::unique_ptr<cv::VideoCapture> m_pInputVideo = nullptr;
    QList<QCameraInfo> m_cameras;
};
#endif // OBJECTDETECTOR_H
