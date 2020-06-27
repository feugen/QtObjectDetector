#include "qtobjectdetector.h"
#include "ui_qtobjectdetector.h"


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
    const int indexBGR888 = formatList.indexOf("BGR888");
    if (indexBGR888 > -1) ui->formatListComboBox->setCurrentIndex(indexBGR888);

    ui->autoSizeCheckBox->setChecked(m_photoLoader->getAutoScale());
    ui->fileNameLabel->setText(m_photoLoader->getFileInfo().fileName());
    ui->xSpinBox->setValue(static_cast<int>(m_photoLoader->getXSize()));
    ui->ySpinBox->setValue(static_cast<int>(m_photoLoader->getYSize()));

    connect(m_fileDialog.get(), &QFileDialog::fileSelected, this, &QtObjectDetector::on_fileSelected);
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
    const auto filePath = m_photoLoader->getFileInfo().absoluteFilePath().toUtf8();
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
