#ifndef OBJECTDETECTOR_H
#define OBJECTDETECTOR_H

#include <QMainWindow>
#include <QFileDialog>
#include <QCamera>
#include <memory>
#include "base.h"
#include "photoloader.h"
#include "videoloader.h"
#include "pipelinehandler.h"

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
    void applyThreshold(double threshold, double thresholdMax, Base::e_OpenCVThresholdType type);
    void applyAdaptiveThreshold(double maxValue, Base::e_OpenCVAdaptivThresholdMethod adaptiveMethod, Base::e_OpenCVAdaptiveThresholdType thresholdType, Base::e_OpenCVBlockSize blockSize, double C);
    void applyMedianBlur(Base::e_OpenCVKSize ksize);
    void applyBlur(cv::Size ksize, cv::Point anchor = cv::Point(-1,-1), Base::e_OPenCVBorderType borderType = Base::e_OPenCVBorderType::BORDER_REFLECT_101);
    void applyGaussianBlur(cv::Size ksize, double sigmaX, double sigmaY = 0, Base::e_OPenCVBorderType borderType = Base::e_OPenCVBorderType::BORDER_REFLECT_101);
    void applyBitwiseNot();
    void applyPow(double power);
    void applyErode(cv::Mat kernel, cv::Point anchor=cv::Point(-1,-1), int iterations=1, Base::e_OPenCVBorderType borderType = Base::e_OPenCVBorderType::BORDER_CONSTANT, const cv::Scalar& borderValue = cv::morphologyDefaultBorderValue());
    void applySobel(int ddepth, int dx, int dy, Base::e_OpenCVKSizeExt ksize = Base::e_OpenCVKSizeExt::KSIZEEXT_3, double scale = 1, double delta = 0, Base::e_OPenCVBorderType borderType = Base::e_OPenCVBorderType::BORDER_REFLECT_101);
    void applyLaplacian(int ddepth, Base::e_OpenCVKSizeExt ksize = Base::e_OpenCVKSizeExt::KSIZEEXT_1, double scale = 1, double delta = 0, Base::e_OPenCVBorderType borderType = Base::e_OPenCVBorderType::BORDER_REFLECT_101);
    void applyAddWeighted(double alphaSRC1, double betaSRC2, double gamma, int dtype = -1);

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
    void on_pushButton_ApplyPipelineVideo_clicked();
    void on_pushButton_DeletePipelineVideo_clicked();
    void on_comboBox_PipelineNameSelector_currentIndexChanged(int index);
    void on_comboBox_PipelineNameSelectorVideo_currentIndexChanged(int index);

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
