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

    struct ImageSettings {
        bool autoScale;
        int x;
        int y;
        QImage::Format imageFormat;
        QByteArray filePath;
    };

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

    void applyCvtColor(PhotoLoader::e_ColorFormat selectedColorFormat);

    //Video Part
    void on_videoSelected(const QString &file);
    void on_pushButton_StartCamera_clicked();
    void on_checkBox_LiveCamera_stateChanged(int arg1);
    void on_pushButton_SelectVideo_clicked();
    void on_pushButton_LoadVideo_clicked();
    void on_checkBox_AutoResolution_stateChanged(int arg1);

    void on_pushButton_addToPipeline_clicked();

    void on_pushButton_deleteFromPipeline_clicked();

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
    std::vector<std::pair<cv::Mat, PhotoLoader::e_ColorFormat>> m_imagePipeline;
    std::vector<std::function<void()>> m_functionPipeline;
    std::function<void()> m_lastFunction;
    QString m_lastFunctionString;
    cv::Mat m_loadedImage;
    //Video Part
    std::unique_ptr<VideoLoader> m_pVideoLoader = nullptr;
    std::unique_ptr<QFileDialog> m_pVideoDialog = nullptr;
    std::unique_ptr<QCamera> m_pCameraActive = nullptr;
    std::unique_ptr<cv::Mat> m_pOutputVideoImage = nullptr;
    std::unique_ptr<cv::VideoCapture> m_pInputVideo = nullptr;
    QList<QCameraInfo> m_cameras;
    ImageSettings m_imageSettings;
};
#endif // OBJECTDETECTOR_H
