#include "qtobjectdetector.h"
#include "ui_qtobjectdetector.h"

#include <QImage>
#include <QPixmap>
#include <QGraphicsVideoItem>
#include <QDebug>
#include <QString>
#include <QMetaEnum>
#include <QByteArray>


QtObjectDetector::QtObjectDetector(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QtObjectDetector)
    , m_pPipelineHandler(new PipelineHandler(this))
    , m_pPhotoLoader(new PhotoLoader(this))
    , m_pImageDialog(new QFileDialog(this))
    , m_pVideoLoader(new VideoLoader(this))
    , m_pVideoDialog(new QFileDialog(this))
    , m_cameras(m_pVideoLoader->CameraInfo().availableCameras())
{
    ui->setupUi(this);

    for (const QCameraInfo &cameraInfo : m_cameras){
        qDebug() << cameraInfo.deviceName() << "found";
        ui->comboBox_LiveCamera->addItem(cameraInfo.deviceName());
        ui->checkBox_LiveCamera->setEnabled(true);
    }

    const auto formatList = m_pPhotoLoader->getFormatList();
    ui->formatListComboBox->insertItems(0, formatList);
    const int indexBGR888 = formatList.indexOf("BGR888");
    if (indexBGR888 > -1) ui->formatListComboBox->setCurrentIndex(indexBGR888);

    ui->checkBox_autoSize->setChecked(m_pPhotoLoader->getAutoSize());
    ui->checkBox_autoScale->setChecked(m_pPhotoLoader->getAutoScale());
    ui->fileNameLabel->setText(m_pPhotoLoader->getFileInfo().fileName());
    ui->xSpinBox->setValue(static_cast<int>(m_pPhotoLoader->getXSize()));
    ui->ySpinBox->setValue(static_cast<int>(m_pPhotoLoader->getYSize()));

    QStringList functionsList;
    const QMetaEnum metaEnum = QMetaEnum::fromType<PipelineHandler::e_OpenCVFunction>();

    for (int i = 0; i < metaEnum.keyCount() ; i++ )
    {
        functionsList += metaEnum.valueToKey(i);
    }
    ui->comboBox_FunctionSelector->addItems(functionsList);

    connect(m_pImageDialog.get(), &QFileDialog::fileSelected, this, &QtObjectDetector::on_fileSelected);
    connect(m_pVideoDialog.get(), &QFileDialog::fileSelected, this, &QtObjectDetector::on_videoSelected);
    connect(ui->actionExit, &QAction::triggered, this, &QApplication::exit);
}

QtObjectDetector::~QtObjectDetector()
{
    delete ui;
}

//General Part


void QtObjectDetector::on_pushButton_ApplyFunction_clicked()
{
    if (!m_imagePipeline.empty())
    {
        const auto selectedFunction = static_cast<PipelineHandler::e_OpenCVFunction>(ui->comboBox_FunctionSelector->currentIndex());

        switch (selectedFunction)
        {
            case PipelineHandler::e_OpenCVFunction::cvtColor:

                auto comboBox = ui->widget_Arguments->findChild<QComboBox*>("comboboxFormat");
                const auto comboBoxValue = comboBox->currentText();

                PhotoLoader::e_ColorFormat selectedColorFormat;
                if(comboBoxValue == "Gray")
                {
                    selectedColorFormat = PhotoLoader::e_ColorFormat::GRAY;
                    applyCvtColor(selectedColorFormat);
                    loadImageToQLabel(m_imagePipeline.size() - 1);
                    m_lastFunction = [this, selectedColorFormat](){applyCvtColor(selectedColorFormat);};
                    m_lastFunctionString = "cvtColor";
                    ui->pushButton_AddToPipeline->setEnabled(true);
                }
                break;
        }
    }
}

//Image Part

void QtObjectDetector::on_selectFileButton_clicked()
{
    m_pImageDialog->open();
}

void QtObjectDetector::on_fileSelected(const QString &file)
{
    m_pPhotoLoader->setFileInfo(file);
    ui->fileNameLabel->setText(m_pPhotoLoader->getFileInfo().fileName());
    ui->loadFilePushButton->setEnabled(true);
}

void QtObjectDetector::on_loadFilePushButton_clicked()
{
    loadImage();
    storeImageSettings();
    loadImageToQLabel(0);
}

void QtObjectDetector::storeImageSettings()
{
    if(!m_imagePipeline.empty())
    {
        m_imageSettings.filePath = m_pPhotoLoader->getFileInfo().absoluteFilePath().toUtf8();
        m_imageSettings.autoSize = ui->checkBox_autoSize->isChecked();
        m_imageSettings.autoScaled = ui->checkBox_autoScale->isChecked();
        m_imageSettings.x = m_imageSettings.autoSize ? m_imagePipeline.at(0).first.cols : (ui->xSpinBox->value() > m_imagePipeline.at(0).first.cols ? m_imagePipeline.at(0).first.cols : ui->xSpinBox->value());
        m_imageSettings.y = m_imageSettings.autoSize  ? m_imagePipeline.at(0).first.rows : (ui->ySpinBox->value() > m_imagePipeline.at(0).first.rows ? m_imagePipeline.at(0).first.rows : ui->ySpinBox->value());
        m_imageSettings.imageFormat = static_cast<QImage::Format>(ui->formatListComboBox->currentIndex());
    }
}

void QtObjectDetector::loadImage()
{
    m_imagePipeline.clear();
    const QString selectedFilter = ui->formatListComboBox->currentText();
    if(selectedFilter == "Indexed8")
    {
        m_originalImage = cv::imread(m_pPhotoLoader->getFileInfo().absoluteFilePath().toUtf8().data(), cv::IMREAD_GRAYSCALE);
        m_imagePipeline.push_back(std::pair<cv::Mat, PhotoLoader::e_ColorFormat>(m_originalImage, PhotoLoader::e_ColorFormat::GRAY));
    }
    else if(selectedFilter == "BGR888")
    {
        m_originalImage = cv::imread(m_pPhotoLoader->getFileInfo().absoluteFilePath().toUtf8().data(), cv::IMREAD_COLOR);
        m_imagePipeline.push_back(std::pair<cv::Mat, PhotoLoader::e_ColorFormat>(m_originalImage, PhotoLoader::e_ColorFormat::COLOR));
    }
}

void QtObjectDetector::loadImageToQLabel(const size_t& storagePosition)
{
    QFile file(m_imageSettings.filePath);
    if (!file.open(QFile::ReadOnly)) return;

    if((m_imagePipeline.size() - 1) >= storagePosition)
    {
        const QImage *imgIn = new QImage(static_cast<uchar*>(m_imagePipeline.at(storagePosition).first.data), m_imageSettings.x, m_imageSettings.y, static_cast<int>(m_imagePipeline.at(storagePosition).first.step), static_cast<QImage::Format>(m_imagePipeline.at(storagePosition).second));   
        QPixmap myPixmap;
        if(m_imageSettings.autoSize)
        {
            const int lineWidth = ui->qLabel_PhotoLoader->lineWidth(); // we need to substract the line width.
            const int bugFix = 1 + 4*lineWidth; //Where does the "1" come from??? Without a box around qlabel (with line width = 1), the 1 fixes the scaling. Why is the Line-width *2 wrong? Fixes needed
            myPixmap = QPixmap::fromImage(*imgIn).scaled(ui->qLabel_PhotoLoader->size().width() - bugFix, ui->qLabel_PhotoLoader->size().height() - bugFix, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        else{
            myPixmap = QPixmap::fromImage(*imgIn).scaled(ui->xSpinBox->value(),ui->ySpinBox->value(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        ui->qLabel_PhotoLoader->setScaledContents(ui->checkBox_autoScale->isChecked());
        ui->qLabel_PhotoLoader->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->qLabel_PhotoLoader->setPixmap(myPixmap);
    }
}

void QtObjectDetector::on_checkBox_autoSize_toggled(bool checked)
{
    if(checked)
    {
        ui->xSpinBox->setEnabled(false);
        ui->ySpinBox->setEnabled(false);
    }
    else
    {
        ui->xSpinBox->setEnabled(true);
        ui->ySpinBox->setEnabled(true);
    }
}


void QtObjectDetector::applyCvtColor(PhotoLoader::e_ColorFormat selectedColorFormat)
{
    const PhotoLoader::e_ColorFormat currentColorFormat = m_imagePipeline.back().second;
    if(currentColorFormat == PhotoLoader::e_ColorFormat::COLOR && selectedColorFormat == PhotoLoader::e_ColorFormat::GRAY) //Only one direction will be supported
    {
        cv::Mat newImage;
        const auto colorFilter = cv::COLOR_BGR2GRAY;

        m_pPipelineHandler->m_cvtColor(m_imagePipeline.back().first, newImage, colorFilter, 0);
        m_imagePipeline.push_back(std::pair<cv::Mat, PhotoLoader::e_ColorFormat>(newImage, selectedColorFormat));
    }
}

void QtObjectDetector::on_pushButton_AddToPipeline_clicked()
{
    m_functionPipeline.push_back(m_lastFunction);
    ui->comboBox_PipelineStepSelector->setEnabled(true);
    ui->comboBox_PipelineStepSelector->addItem(m_lastFunctionString);
    ui->pushButton_AddToPipeline->setEnabled(false);
    ui->pushButton_DeleteFromPipeline->setEnabled(true);

    if(ui->lineEdit_PipelineName->text().length() > 0 && ui->lineEdit_PipelineName->text().at(0).isLetter())
    {
        ui->pushButton_SavePipeline->setEnabled(true);
    }
}

void QtObjectDetector::on_pushButton_DeleteFromPipeline_clicked()
{
    const auto currentIndex = ui->comboBox_PipelineStepSelector->currentIndex();
    ui->comboBox_PipelineStepSelector->removeItem(currentIndex);
    if(static_cast<int>(m_functionPipeline.size()) > currentIndex)
    {
        m_functionPipeline.erase(m_functionPipeline.begin() + currentIndex);
        m_imagePipeline.erase(m_imagePipeline.begin() + currentIndex);
    }
    if(m_functionPipeline.empty())
    {
        ui->comboBox_PipelineStepSelector->setEnabled(false);
        ui->pushButton_DeleteFromPipeline->setEnabled(false);
        ui->pushButton_ApplyPipeline->setEnabled(false);
        ui->pushButton_SavePipeline->setEnabled(false);
    }
}

void QtObjectDetector::on_pushButton_SavePipeline_clicked()
{
    const auto pipelineName = ui->lineEdit_PipelineName->text();
    if(pipelineName.at(0).isLetter() && !m_functionPipeline.empty())
    {
        const auto pipelinePair = std::pair<std::vector<std::function<void()>>, QString>(m_functionPipeline, pipelineName);
        m_availablePipelines.push_back(pipelinePair);
        ui->comboBox_PipelineNameSelector->setEnabled(true);
        ui->pushButton_ApplyPipeline->setEnabled(true);
        ui->pushButton_DeletePipeline->setEnabled(true);
        ui->comboBox_PipelineNameSelector->addItem(pipelineName);
    }
}

void QtObjectDetector::on_pushButton_DeletePipeline_clicked()
{
    const int currentIndex = ui->comboBox_PipelineNameSelector->currentIndex();
    if(currentIndex >= 0)
    {
        ui->comboBox_PipelineNameSelector->removeItem(currentIndex);
        if (static_cast<int>(m_availablePipelines.size() - 1) >= currentIndex)
        {
            m_availablePipelines.erase(m_availablePipelines.begin() + currentIndex);
        }
    }
}

void QtObjectDetector::on_lineEdit_PipelineName_textChanged(const QString &arg1)
{
    if(arg1.at(0).isLetter() && ui->comboBox_PipelineStepSelector->count() > 0)
    {
        ui->pushButton_SavePipeline->setEnabled(true);
    }
}

void QtObjectDetector::on_pushButton_ApplyPipeline_clicked()
{
    const QString selectedPipeline = ui->comboBox_PipelineNameSelector->currentText();
    for (const auto& pipeline : m_availablePipelines)
    {
        if(pipeline.second == selectedPipeline)
        {
            const auto functionPipeline = pipeline.first;

            for(const auto& function : functionPipeline)
            {
                function();
            }
        }
    }
    if(m_imagePipeline.size() > 0)
    {
        loadImageToQLabel(m_imagePipeline.size()-1);
    }
}

void QtObjectDetector::on_comboBox_FunctionSelector_currentIndexChanged(const QString &arg1)
{

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    ui->widget_Arguments->setLayout(layout);

    if(arg1 == "cvtColor")
    {
        QLabel* labelFormat = new QLabel(this);
        labelFormat->setText("Color Format:");

        QComboBox* comboboxFormat = new QComboBox(this);
        comboboxFormat->setObjectName("comboboxFormat");
        comboboxFormat->addItem("Gray");

        ui->widget_Arguments->layout()->addWidget(labelFormat);
        ui->widget_Arguments->layout()->addWidget(comboboxFormat);
    }
}

/*
////////////////////////////////////////////////Video Part////////////////////////////////////////////
*/

void QtObjectDetector::on_videoSelected(const QString &file)
{
    m_pVideoLoader->setFileInfo(file);
    ui->label_videoName->setText(m_pVideoLoader->getFileInfo().fileName());
    ui->pushButton_LoadVideo->setEnabled(true);
}

void QtObjectDetector::on_pushButton_SelectVideo_clicked()
{
    m_pVideoDialog->open();
}

void QtObjectDetector::on_pushButton_LoadVideo_clicked()
{
    const auto filepath = m_pVideoLoader->getFileInfo().absoluteFilePath().toUtf8();

    cv::namedWindow("Frame", cv::WINDOW_AUTOSIZE);

    if(!m_pInputVideo)
    {
        m_pInputVideo = std::make_unique<cv::VideoCapture>();
    }

    if(!m_pOutputVideoImage)
    {
        m_pOutputVideoImage = std::make_unique<cv::Mat>();
    }

    if (!m_pInputVideo->open(filepath.data(), cv::CAP_ANY)) return;

    const double fps = m_pInputVideo.get()->get(cv::CAP_PROP_FPS);

    qDebug() << fps << "Fps";

    int i = 0;

    QGraphicsScene * scene = new QGraphicsScene();
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem();
    QPixmap pixmap;

    ui->graphicsView_VideoLoader->setScene(scene);
    scene->addItem(item);

    for(;;)
    {
        qDebug() << i;
        m_pInputVideo->read(*m_pOutputVideoImage.get());
        if (m_pOutputVideoImage.get()->empty())
        {
            qDebug() << "Empty Image, end video";
            break;
        }
        QImage qimg((*(m_pOutputVideoImage.get())).data, (*(m_pOutputVideoImage.get())).cols, (*(m_pOutputVideoImage.get())).rows, static_cast<int>((*(m_pOutputVideoImage.get())).step), QImage::Format_RGB888);
        pixmap = QPixmap::fromImage(qimg.rgbSwapped());
        item->setPixmap(pixmap);

        //ui->graphicsView_VideoLoader->scene()->addItem(pixmap);

        //cv::imshow("Frame", *m_outputVideoImage.get()); //external window
        i++;

        cv::waitKey(1000 / static_cast<int>(fps));
    }
    m_pInputVideo->release();
    cv::destroyAllWindows();
}

void QtObjectDetector::stopCamera() const
{
    if(m_pCameraActive){
        m_pCameraActive->stop();
        ui->pushButton_StartCamera->setText("Start Camera");
        ui->spinBox_CameraXResolution->setEnabled(true);
        ui->spinBox_CameraYResolution->setEnabled(true);
        ui->comboBox_LiveCamera->setEnabled(true);
        ui->checkBox_LiveCamera->setEnabled(true);
    }
}



void QtObjectDetector::on_pushButton_StartCamera_clicked()
{
    if(m_pCameraActive == nullptr || (m_pCameraActive->status() != QCamera::Status::ActiveStatus))
    {
        const auto cameraSelected = ui->comboBox_LiveCamera->currentText();
        for (const QCameraInfo &cameraInfo : m_cameras){
            if(cameraInfo.deviceName() == cameraSelected)
            {
                m_pCameraActive = std::make_unique<QCamera>(new QCamera(cameraInfo));
                qDebug() << cameraInfo.description();
            }
        }

        if(m_pCameraActive == nullptr)
        {
            qDebug() << "Something went wrong on camera selection";
            return;
        }

        QGraphicsScene * scene = new QGraphicsScene();
        QGraphicsVideoItem *item = new QGraphicsVideoItem();


        item->setSize(QSizeF(ui->spinBox_CameraXResolution->text().toInt(),ui->spinBox_CameraYResolution->text().toInt()));

        ui->graphicsView_VideoLoader->setScene(scene);
        scene->addItem(item);
        ui->graphicsView_VideoLoader->show();

        m_pCameraActive->setViewfinder(item);
        m_pCameraActive->start();

        ui->pushButton_StartCamera->setText("Stop Camera");
        ui->spinBox_CameraXResolution->setEnabled(false);
        ui->spinBox_CameraYResolution->setEnabled(false);
        ui->comboBox_LiveCamera->setEnabled(false);
        ui->checkBox_LiveCamera->setEnabled(false);
    }
    else
    {
        stopCamera();
    }

}

void QtObjectDetector::on_checkBox_LiveCamera_stateChanged(int arg1)
{
    if(static_cast<bool>(arg1) == true)
    {
        ui->pushButton_StartCamera->setEnabled(true);
        ui->comboBox_LiveCamera->setEnabled(true);
        ui->pushButton_SelectVideo->setEnabled(false);
    }
    else
    {
        ui->pushButton_StartCamera->setEnabled(false);
        ui->comboBox_LiveCamera->setEnabled(false);
        ui->spinBox_CameraXResolution->setEnabled(false);
        ui->spinBox_CameraYResolution->setEnabled(false);
        ui->pushButton_SelectVideo->setEnabled(true);
    }
}


void QtObjectDetector::on_checkBox_AutoResolution_stateChanged(int arg1)
{
    if(static_cast<bool>(arg1) == true)
    {
        ui->spinBox_CameraXResolution->setEnabled(false);
        ui->spinBox_CameraYResolution->setEnabled(false);
    }
    else
    {
        ui->spinBox_CameraXResolution->setEnabled(true);
        ui->spinBox_CameraYResolution->setEnabled(true);
    }
}
