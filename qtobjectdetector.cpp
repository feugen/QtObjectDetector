#include "qtobjectdetector.h"
#include "ui_qtobjectdetector.h"
#include <opencv2/opencv.hpp>

#include <QImage>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>


QtObjectDetector::QtObjectDetector(QWidget *parent)
    : QMainWindow(parent)
    , m_photoLoader(new PhotoLoader(this))
    , m_fileDialog(new QFileDialog(this))
    , ui(new Ui::QtObjectDetector)
{
    ui->setupUi(this);

    const auto formatList = m_photoLoader->getFormatList();
    ui->formatListComboBox->insertItems(0, formatList);
    const int indexBGR888 = formatList.indexOf("Format_BGR888");
    if (indexBGR888 > -1) ui->formatListComboBox->setCurrentIndex(indexBGR888);

    ui->autoSizeCheckBox->setChecked(m_photoLoader->getAutoScale());
    ui->fileNameLabel->setText(m_photoLoader->getFileInfo().fileName());
    ui->xSpinBox->setValue(static_cast<int>(m_photoLoader->getXSize()));
    ui->ySpinBox->setValue(static_cast<int>(m_photoLoader->getYSize()));

    connect(m_fileDialog, &QFileDialog::fileSelected, this, &QtObjectDetector::on_fileSelected);
}

QtObjectDetector::~QtObjectDetector()
{
    delete ui;
}


void QtObjectDetector::on_selectFileButton_clicked()
{
    m_fileDialog->open();
}

void QtObjectDetector::on_fileSelected(const QString &file)
{
    m_photoLoader->setFileInfo(file);
    ui->fileNameLabel->setText(m_photoLoader->getFileInfo().fileName());
    ui->loadFilePushButton->setEnabled(true);
}

void QtObjectDetector::on_loadFilePushButton_clicked()
{
    const auto test = m_photoLoader->getFileInfo().absoluteFilePath().toUtf8();
    const cv::Mat inputImage = cv::imread(test.data());

    const bool autoScale = ui->autoSizeCheckBox->isChecked();
    const int x = autoScale ? inputImage.cols : (ui->xSpinBox->value() > inputImage.cols ? inputImage.cols : ui->xSpinBox->value());
    const int y = autoScale ? inputImage.rows : (ui->ySpinBox->value() > inputImage.rows ? inputImage.rows : ui->ySpinBox->value());

    const int formatIndex = ui->formatListComboBox->currentIndex();

    const QImage * imgIn = new QImage(static_cast<uchar*>(inputImage.data), x, y, static_cast<int>(inputImage.step), static_cast<QImage::Format>(formatIndex));
    QGraphicsPixmapItem * item = new QGraphicsPixmapItem(QPixmap::fromImage(*imgIn));
    QGraphicsScene* scene = new QGraphicsScene(this);

    scene->addItem(item);
    ui->graphicsView_PhotoLoader->setScene(scene);
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
