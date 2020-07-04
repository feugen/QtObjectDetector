#include "qtobjectdetector.h"
#include "ui_qtobjectdetector.h"

#include <QImage>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QGraphicsVideoItem>
#include <QDebug>
#include <QString>
#include <QMetaEnum>
#include <QVariant>

QtObjectDetector::QtObjectDetector(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QtObjectDetector)
    , m_pPipelineHandler(new PipelineHandler(this))
    , m_pScene(new QGraphicsScene(this))
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

    ui->autoSizeCheckBox->setChecked(m_pPhotoLoader->getAutoScale());
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
}

QtObjectDetector::~QtObjectDetector()
{
    delete ui;
}

//General Part


//Todo
void QtObjectDetector::on_pushButton_ApplyFunction_clicked()
{
    const auto selectedFunction = static_cast<PipelineHandler::e_OpenCVFunction>(ui->comboBox_FunctionSelector->currentIndex());

    switch (selectedFunction) {
    case PipelineHandler::e_OpenCVFunction::cvtColor:
        cv::Mat tempImage;
        m_pPipelineHandler->m_cvtColor(m_inputImage, tempImage, cv::COLOR_BGR2GRAY, 0);
        m_inputImage = tempImage;

        const bool autoScale = ui->autoSizeCheckBox->isChecked();
        const int x = autoScale ? m_inputImage.cols : (ui->xSpinBox->value() > m_inputImage.cols ? m_inputImage.cols : ui->xSpinBox->value());
        const int y = autoScale ? m_inputImage.rows : (ui->ySpinBox->value() > m_inputImage.rows ? m_inputImage.rows : ui->ySpinBox->value());

        QImage * imgIn2 = new QImage();
        imgIn2->loadFromData(static_cast<uchar*>(m_inputImage.data), static_cast<int>(m_inputImage.total()));



        const QImage * imgIn = new QImage(static_cast<uchar*>(m_inputImage.data), x, y, static_cast<int>(m_inputImage.step), QImage::Format::Format_Mono);
        QGraphicsPixmapItem * item = new QGraphicsPixmapItem(QPixmap::fromImage(*imgIn));
        m_pScene->addItem(item);
        ui->graphicsView_PhotoLoader->setScene(m_pScene);
        ui->graphicsView_PhotoLoader->show();

        break;
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
    const auto filePath = m_pPhotoLoader->getFileInfo().absoluteFilePath().toUtf8();
    m_inputImage = cv::imread(filePath.data());
    loadImage();
}

void QtObjectDetector::loadImage()
{
    const bool autoScale = ui->autoSizeCheckBox->isChecked();
    const int x = autoScale ? m_inputImage.cols : (ui->xSpinBox->value() > m_inputImage.cols ? m_inputImage.cols : ui->xSpinBox->value());
    const int y = autoScale ? m_inputImage.rows : (ui->ySpinBox->value() > m_inputImage.rows ? m_inputImage.rows : ui->ySpinBox->value());
    const int formatIndex = ui->formatListComboBox->currentIndex();

    const QImage * imgIn = new QImage(static_cast<uchar*>(m_inputImage.data), x, y, static_cast<int>(m_inputImage.step), static_cast<QImage::Format>(formatIndex));
    QGraphicsPixmapItem * item = new QGraphicsPixmapItem(QPixmap::fromImage(*imgIn));

    m_pScene->addItem(item);
    ui->graphicsView_PhotoLoader->setScene(m_pScene);
    ui->graphicsView_PhotoLoader->show();
}

void QtObjectDetector::on_autoSizeCheckBox_toggled(bool checked)
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


//Video Part

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

    //const auto buildInfo = cv::getBuildInformation();
    //qDebug() << QString::fromStdString(buildInfo);

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
