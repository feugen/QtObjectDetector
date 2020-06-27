#include "qtobjectdetector.h"
#include "ui_qtobjectdetector.h"

#include <QImage>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QDebug>
#include <QString>


QtObjectDetector::QtObjectDetector(QWidget *parent)
    : QMainWindow(parent)
    , m_pPhotoLoader(new PhotoLoader(this))
    , m_pVideoLoader(new VideoLoader(this))
    , m_pImageDialog(new QFileDialog(this))
    , m_pVideoDialog(new QFileDialog(this))
    , m_cameras(m_pVideoLoader->CameraInfo().availableCameras())
    , ui(new Ui::QtObjectDetector)
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

    connect(m_pImageDialog.get(), &QFileDialog::fileSelected, this, &QtObjectDetector::on_fileSelected);
    connect(m_pVideoDialog.get(), &QFileDialog::fileSelected, this, &QtObjectDetector::on_videoSelected);
}

QtObjectDetector::~QtObjectDetector()
{
    delete ui;
}


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

void QtObjectDetector::on_videoSelected(const QString &file)
{
    m_pVideoLoader->setFileInfo(file);
    ui->label_videoName->setText(m_pVideoLoader->getFileInfo().fileName());
    ui->pushButton_LoadVideo->setEnabled(true);
}

void QtObjectDetector::on_loadFilePushButton_clicked()
{
    const auto filePath = m_pPhotoLoader->getFileInfo().absoluteFilePath().toUtf8();
    m_inputImage = cv::imread(filePath.data());
    loadImage();
}

void QtObjectDetector::on_pushButton_SelectVideo_clicked()
{
    m_pVideoDialog->open();
}

void QtObjectDetector::loadImage()
{
    const bool autoScale = ui->autoSizeCheckBox->isChecked();
    const int x = autoScale ? m_inputImage.cols : (ui->xSpinBox->value() > m_inputImage.cols ? m_inputImage.cols : ui->xSpinBox->value());
    const int y = autoScale ? m_inputImage.rows : (ui->ySpinBox->value() > m_inputImage.rows ? m_inputImage.rows : ui->ySpinBox->value());
    const int formatIndex = ui->formatListComboBox->currentIndex();

    const QImage * imgIn = new QImage(static_cast<uchar*>(m_inputImage.data), x, y, static_cast<int>(m_inputImage.step), static_cast<QImage::Format>(formatIndex));
    QGraphicsPixmapItem * item = new QGraphicsPixmapItem(QPixmap::fromImage(*imgIn));
    QGraphicsScene* scene = new QGraphicsScene(this);

    scene->addItem(item);
    ui->graphicsView_PhotoLoader->setScene(scene);
    ui->graphicsView_PhotoLoader->show();
}

void QtObjectDetector::on_pushButton_LoadVideo_clicked()
{
    const auto filepath = m_pVideoLoader->getFileInfo().absoluteFilePath().toUtf8();
    m_inputVideo.open(filepath.data());

    //Todo, not working
    while(m_inputVideo.isOpened()){
        m_inputVideo.read(m_outputVideoImage);
        cv::imshow("test", m_outputVideoImage);
    }

}

void QtObjectDetector::stopCamera()
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
    }
    else
    {
        ui->pushButton_StartCamera->setEnabled(false);
        ui->comboBox_LiveCamera->setEnabled(false);
        ui->spinBox_CameraXResolution->setEnabled(false);
        ui->spinBox_CameraYResolution->setEnabled(false);
    }
}

