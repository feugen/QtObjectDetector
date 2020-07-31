#include "qtobjectdetector.h"
#include "ui_qtobjectdetector.h"
#include "layouthandler.h"

#include <QPixmap>
#include <QGraphicsVideoItem>
#include <QAbstractEventDispatcher>
#include <QDebug>

QtObjectDetector::QtObjectDetector(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QtObjectDetector)
    , m_pPipelineHandler(new PipelineHandler(this))
    , m_pPhotoLoader(new PhotoLoader(this))
    , m_pImageDialog(new QFileDialog(this))
    , m_pVideoLoader(new VideoLoader(this))
    , m_pVideoDialog(new QFileDialog(this))
    , m_pScene(new QGraphicsScene(this))
    , m_Cameras(m_pVideoLoader->CameraInfo().availableCameras())
{
    ui->setupUi(this);

    for (const QCameraInfo &cameraInfo : m_Cameras){
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
    const QMetaEnum metaEnum = QMetaEnum::fromType<Base::e_OpenCVFunction>();

    for (int i = 0; i < metaEnum.keyCount() ; i++ )
    {
        functionsList += metaEnum.valueToKey(i);
    }
    ui->comboBox_FunctionSelector->addItems(functionsList);

    connect(m_pImageDialog.get(), &QFileDialog::fileSelected, this, &QtObjectDetector::on_fileSelected);
    connect(m_pVideoDialog.get(), &QFileDialog::fileSelected, this, &QtObjectDetector::on_videoSelected);
    connect(this, &QtObjectDetector::newVideoImage , this, &QtObjectDetector::on_newVideoImage);
    connect(ui->actionExit, &QAction::triggered, this, &QApplication::exit);
}

QtObjectDetector::~QtObjectDetector()
{
    delete ui;
}

//General Part


void QtObjectDetector::on_pushButton_ApplyFunction_clicked()
{
    if (!m_pPipelineHandler->getImagePipeline().empty())
    {
        const auto selectedFunction = static_cast<Base::e_OpenCVFunction>(ui->comboBox_FunctionSelector->currentIndex());

        switch (selectedFunction)
        {
            case Base::e_OpenCVFunction::CvtColor:
            {
                const auto arg1 = ui->widget_Arguments->findChild<QComboBox*>("comboboxFormat");

                if(arg1)
                {
                    const auto arg1Value = arg1->currentText();
                    Base::e_OpenCVColorFormat selectedColorFormat;
                    if(arg1Value == "Gray")
                    {
                        selectedColorFormat = Base::e_OpenCVColorFormat::GRAY;
                        m_lastFunction = [this, selectedColorFormat](){applyCvtColor(selectedColorFormat);};
                    }
                }
                break;
            }

            case Base::e_OpenCVFunction::Threshold:
            {
                const auto arg1 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("spinboxTresholdValue");
                const auto arg2 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("spinboxTresholdValueMax");
                const auto arg3 = ui->widget_Arguments->findChild<QComboBox*>("comboboxThreshType");

                if(arg1 && arg2 && arg3)
                {
                    const auto arg1Value = arg1->value();
                    const auto arg2Value = arg2->value();
                    const auto arg3Value = arg3->currentIndex();
                    const auto arg3ValueText = arg3->currentText();

                    //Verify data
                    const auto enum3Value = static_cast<Base::e_OpenCVThresholdType>(arg3Value);
                    assert(Base::QEnumToQString(enum3Value) == arg3ValueText);

                    m_lastFunction = [=](){applyThreshold(arg1Value, arg2Value, enum3Value);};
                }
                break;
            }

            case Base::e_OpenCVFunction::AdaptiveThreshold:
            {
                const auto arg1 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("spinboxTresholdValueMax");
                const auto arg2 = ui->widget_Arguments->findChild<QComboBox*>("comboboxAdaptiveMethod");
                const auto arg3 = ui->widget_Arguments->findChild<QComboBox*>("comboboxThreshType");
                const auto arg4 = ui->widget_Arguments->findChild<QComboBox*>("comboboxBlockSize");
                const auto arg5 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("spinboxConstantC");

                if(arg1 && arg2 && arg3 && arg4 && arg5)
                {
                    const auto arg1Value = arg1->value();
                    const auto arg2Value = arg2->currentIndex();
                    const auto arg2ValueText = arg2->currentText();
                    const auto arg3Value = arg3->currentIndex();
                    const auto arg3ValueText = arg3->currentText();
                    const auto arg4Value = arg4->currentText().toInt();
                    const auto arg4ValueText = arg4->currentText();
                    const auto arg5Value = arg5->value();

                    //Verify data
                    const auto enumValue2 = static_cast<Base::e_OpenCVAdaptivThresholdMethod>(arg2Value);
                    assert(Base::QEnumToQString(enumValue2) == arg2ValueText);
                    const auto enumValue3 = static_cast<Base::e_OpenCVAdaptiveThresholdType>(arg3Value);
                    assert(Base::QEnumToQString(enumValue3) == arg3ValueText);
                    const auto enumValue4 = static_cast<Base::e_OpenCVBlockSize>(arg4Value);
                    assert(Base::QEnumToQString(enumValue4).mid(10) == arg4ValueText);

                    m_lastFunction = [=](){applyAdaptiveThreshold(arg1Value, enumValue2, enumValue3, enumValue4, arg5Value);};
                }
                break;
            }

            case Base::e_OpenCVFunction::MedianBlur:
            {
                const auto arg1 = ui->widget_Arguments->findChild<QComboBox*>("comboboxKSize");
                if(arg1)
                {
                    const auto arg1Value = arg1->currentText();
                    bool isOk = false;
                    const auto arg1ValueInt = arg1Value.toInt(&isOk);
                    if(isOk)
                    {
                        //Verify data
                        const auto enumValue1 = static_cast<Base::e_OpenCVKSize>(arg1ValueInt);
                        assert(Base::QEnumToQString(enumValue1).mid(6) == arg1Value);

                        m_lastFunction = [=](){applyMedianBlur(enumValue1);};
                    }
                }
                break;
            }
            case Base::e_OpenCVFunction::Blur:
            {
                const auto arg1 = ui->widget_Arguments->findChild<QSpinBox*>("spinBoxKSizeWidth");
                const auto arg2 = ui->widget_Arguments->findChild<QSpinBox*>("spinBoxKSizeHeight");
                const auto arg3 = ui->widget_Arguments->findChild<QComboBox*>("comboboxBorderType");

                if(arg1 && arg2 && arg3)
                {
                    const auto arg1Value = arg1->value();
                    const auto arg2Value = arg2->value();
                    const auto arg3Value = arg3->currentIndex();
                    const auto arg3ValueText = arg3->currentText();

                    auto size = cv::Size(arg1Value, arg2Value);
                    auto point = cv::Point(-1,-1); //Look at OpenCv documentation

                    //Verify data
                    const auto enumValue3 = static_cast<Base::e_OPenCVBorderType>(arg3Value);
                    assert(Base::QEnumToQString(enumValue3) == arg3ValueText);

                    m_lastFunction = [=](){applyBlur(size, point, enumValue3);};
                }
                break;
            }
            case Base::e_OpenCVFunction::GaussianBlur:
            {
                const auto arg1 = ui->widget_Arguments->findChild<QSpinBox*>("spinBoxKSizeWidth");
                const auto arg2 = ui->widget_Arguments->findChild<QSpinBox*>("spinBoxKSizeHeight");
                const auto arg3 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("spinBoxSigmaX");
                const auto arg4 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("spinBoxSigmaY");
                const auto arg5 = ui->widget_Arguments->findChild<QComboBox*>("comboboxBorderType");

                if(arg1 && arg2 && arg3 && arg4 && arg5)
                {
                    const auto arg1Value = arg1->value();
                    const auto arg2Value = arg2->value();
                    const auto arg3Value = arg3->value();
                    const auto arg4Value = arg4->value();
                    const auto arg5Value = arg5->currentIndex();
                    const auto arg5ValueText = arg5->currentText();

                    auto size = cv::Size(arg1Value, arg2Value);

                    //Verify data
                    const auto enumValue5 = static_cast<Base::e_OPenCVBorderType>(arg5Value);
                    assert(Base::QEnumToQString(enumValue5) == arg5ValueText);

                    m_lastFunction = [=](){applyGaussianBlur(size, arg3Value, arg4Value, enumValue5);};
                }
                break;
            }

            case Base::e_OpenCVFunction::BitwiseNot:
            {
                m_lastFunction = [=](){applyBitwiseNot();};
                break;
            }
            case Base::e_OpenCVFunction::Pow:
            {
                const auto arg1 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("spinBoxPower");

                if(arg1)
                {
                    const auto arg1Value = arg1->value();

                    m_lastFunction = [=](){applyPow(arg1Value);};
                }
                break;
            }

            case Base::e_OpenCVFunction::Erode:
            {
                const auto arg1 = ui->widget_Arguments->findChild<QSpinBox*>("spinBoxIterations");
                const auto arg2 = ui->widget_Arguments->findChild<QComboBox*>("comboboxBorderType");
                const auto arg3 = ui->widget_Arguments->findChild<QComboBox*>("comboboxMorphShapes");
                const auto arg4 = ui->widget_Arguments->findChild<QComboBox*>("comboboxErosionSize");

                if(arg1 && arg2 && arg3 && arg4)
                {
                    const auto arg1Value = arg1->value();
                    const auto arg2Value = arg2->currentIndex();
                    const auto arg2ValueText = arg2->currentText();
                    const auto arg3Value = arg3->currentIndex();
                    const auto arg3ValueText = arg3->currentText();
                    const auto arg4Value = arg4->currentIndex()+1;
                    const auto arg4ValueText = arg4->currentText();

                    //Verify data
                    const auto enumValue2 = static_cast<Base::e_OPenCVBorderType>(arg2Value);
                    assert(Base::QEnumToQString(enumValue2) == arg2ValueText);
                    const auto enumValue3 = static_cast<Base::e_OPenCVMorphShapes>(arg3Value);
                    assert(Base::QEnumToQString(enumValue3) == arg3ValueText);
                    const auto enumValue4 = static_cast<Base::e_OpenCVErosionSize>(arg4Value);
                    assert(Base::QEnumToQString(enumValue4) == arg4ValueText);

                    cv::Mat element = cv::getStructuringElement(enumValue3, cv::Size( 2*enumValue4 + 1, 2*enumValue4+1 ), cv::Point( enumValue4, enumValue4));
                    m_lastFunction = [=](){applyErode(element, cv::Point(-1,-1), arg1Value, enumValue2, cv::morphologyDefaultBorderValue());};
                }

                break;
            }
            case Base::e_OpenCVFunction::Sobel:
            {
                const auto arg1 = ui->widget_Arguments->findChild<QSpinBox*>("spinBoxDx");
                const auto arg2 = ui->widget_Arguments->findChild<QSpinBox*>("spinBoxDy");
                const auto arg3 = ui->widget_Arguments->findChild<QComboBox*>("comboboxKSize");
                const auto arg4 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("spinBoxScale");
                const auto arg5 = ui->widget_Arguments->findChild<QComboBox*>("comboboxBorderType");

                if(arg1 && arg2 && arg3 && arg4 && arg5)
                {
                    const auto arg1Value = arg1->value();
                    const auto arg2Value = arg2->value();
                    const auto arg3Value = arg3->currentIndex();
                    const auto arg3ValueInt = 2*arg3Value+1;
                    const auto arg3ValueText = arg3->currentText();
                    const auto arg4Value = arg4->value();
                    const auto arg5Value = arg5->currentIndex();
                    const auto arg5ValueText = arg5->currentText();

                    //Verify data
                    const auto enumValue3 = static_cast<Base::e_OpenCVKSizeExt>(arg3ValueInt);
                    assert(Base::QEnumToQString(enumValue3) == arg3ValueText);
                    const auto enumValue5 = static_cast<Base::e_OPenCVBorderType>(arg5Value);
                    assert(Base::QEnumToQString(enumValue5) == arg5ValueText);

                    m_lastFunction = [=](){applySobel(-1, arg1Value, arg2Value, enumValue3, arg4Value, 0, enumValue5);};
                }
                break;
            }

            case Base::e_OpenCVFunction::Laplacian:
            {
                const auto arg1 = ui->widget_Arguments->findChild<QComboBox*>("comboboxKSize");
                const auto arg2 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("spinBoxScale");
                const auto arg3 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("spinBoxDelta");
                const auto arg4 = ui->widget_Arguments->findChild<QComboBox*>("comboboxBorderType");

                if(arg1 && arg2 && arg3 && arg4)
                {
                    const auto arg1Value = arg1->currentIndex();
                    const auto arg1ValueInt = 2*arg1Value+1;
                    const auto arg1ValueText = arg1->currentText();
                    const auto arg2Value = arg2->value();
                    const auto arg3Value = arg3->value();
                    const auto arg4Value = arg4->currentIndex();
                    const auto arg4ValueText = arg4->currentText();

                    //Verify data
                    const auto enumValue1 = static_cast<Base::e_OpenCVKSizeExt>(arg1ValueInt);
                    assert(Base::QEnumToQString(enumValue1) == arg1ValueText);
                    const auto enumValue4 = static_cast<Base::e_OPenCVBorderType>(arg4Value);
                    assert(Base::QEnumToQString(enumValue4) == arg4ValueText);

                    m_lastFunction = [=](){applyLaplacian(-1, enumValue1, arg2Value, arg3Value, enumValue4);};
                }
                break;
            }
            case Base::e_OpenCVFunction::AddWeighted:
            {
                const auto arg1 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("spinBoxAlpha");
                const auto arg2 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("spinBoxBeta");
                const auto arg3 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("spinBoxGamma");

                if(arg1 && arg2 && arg3)
                {
                    const auto arg1Value = arg1->value();
                    const auto arg2Value = arg2->value();
                    const auto arg3Value = arg3->value();

                    m_lastFunction = [=](){applyAddWeighted(arg1Value, arg2Value, arg3Value, -1);};
                }
                break;
            }
        }

        if(m_lastFunction)
        {
            m_lastFunctionString = Base::QEnumToQString(selectedFunction);
            m_lastFunction();
            loadImageToQLabel(m_pPipelineHandler->getImagePipeline().size() - 1);
            ui->pushButton_AddToPipeline->setEnabled(true);
        }
    }
}

Base::e_OpenCVColorFormat QtObjectDetector::getColorFormat(cv::Mat mat, bool BGRtoRGB) const
{
    const auto chanels = mat.channels();
    Base::e_OpenCVColorFormat colorFormat = Base::e_OpenCVColorFormat::UNKNOWN;

    if(chanels == 1)
    {
        colorFormat = Base::e_OpenCVColorFormat::GRAY;
    }
    else if(chanels == 3)
    {
        if(BGRtoRGB)
        {
            colorFormat = Base::e_OpenCVColorFormat::COLOR;
        }
        else{
            colorFormat = Base::e_OpenCVColorFormat::COLOR_INV;
        }
    }
    else{
        qDebug() << "Number of video image channels is neither 3 nor 1";
    }
    return colorFormat;
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
    if(!m_pPipelineHandler->getImagePipeline().empty())
    {
        m_imageSettings.filePath = m_pPhotoLoader->getFileInfo().absoluteFilePath().toUtf8();
        m_imageSettings.autoSize = ui->checkBox_autoSize->isChecked();
        m_imageSettings.autoScaled = ui->checkBox_autoScale->isChecked();
        m_imageSettings.x = m_imageSettings.autoSize ? m_pPipelineHandler->getImagePipeline().at(0).first.cols : (ui->xSpinBox->value() > m_pPipelineHandler->getImagePipeline().at(0).first.cols ? m_pPipelineHandler->getImagePipeline().at(0).first.cols : ui->xSpinBox->value());
        m_imageSettings.y = m_imageSettings.autoSize  ? m_pPipelineHandler->getImagePipeline().at(0).first.rows : (ui->ySpinBox->value() > m_pPipelineHandler->getImagePipeline().at(0).first.rows ? m_pPipelineHandler->getImagePipeline().at(0).first.rows : ui->ySpinBox->value());
        m_imageSettings.imageFormat = static_cast<QImage::Format>(ui->formatListComboBox->currentIndex());
    }
}

void QtObjectDetector::loadImage()
{
    m_pPipelineHandler->getImagePipeline().clear();
    const QString selectedFilter = ui->formatListComboBox->currentText();
    if(selectedFilter == "Indexed8")
    {
        m_originalImage = cv::imread(m_pPhotoLoader->getFileInfo().absoluteFilePath().toUtf8().data(), cv::IMREAD_GRAYSCALE);
        m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(m_originalImage, Base::e_OpenCVColorFormat::GRAY));
    }
    else if(selectedFilter == "BGR888")
    {
        m_originalImage = cv::imread(m_pPhotoLoader->getFileInfo().absoluteFilePath().toUtf8().data(), cv::IMREAD_COLOR);
        m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(m_originalImage, Base::e_OpenCVColorFormat::COLOR));
    }
}

void QtObjectDetector::loadImageToQLabel(const size_t& storagePosition)
{
    QFile file(m_imageSettings.filePath);
    if (!file.open(QFile::ReadOnly)) return;

    if((m_pPipelineHandler->getImagePipeline().size() - 1) >= storagePosition)
    {
        qDebug() << "Image Format loaded: " << QString::fromStdString(cv::typeToString(m_pPipelineHandler->getImagePipeline().at(storagePosition).first.type()));
        const QImage *imgIn = new QImage(static_cast<uchar*>(m_pPipelineHandler->getImagePipeline().at(storagePosition).first.data), m_imageSettings.x, m_imageSettings.y, static_cast<int>(m_pPipelineHandler->getImagePipeline().at(storagePosition).first.step), static_cast<QImage::Format>(m_pPipelineHandler->getImagePipeline().at(storagePosition).second));
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


void QtObjectDetector::applyCvtColor(Base::e_OpenCVColorFormat selectedColorFormat)
{
    const Base::e_OpenCVColorFormat currentColorFormat = m_pPipelineHandler->getImagePipeline().back().second;
    if((currentColorFormat == Base::e_OpenCVColorFormat::COLOR || currentColorFormat == Base::e_OpenCVColorFormat::COLOR_INV) && selectedColorFormat == Base::e_OpenCVColorFormat::GRAY) //Only one direction will be supported
    {
        cv::Mat newImage;
        const auto colorFilter = currentColorFormat == Base::e_OpenCVColorFormat::COLOR ? cv::COLOR_BGR2GRAY : cv::COLOR_RGB2GRAY;
        try{
            m_pPipelineHandler->getCvtColor()(m_pPipelineHandler->getImagePipeline().back().first, newImage, colorFilter, 0);
        }
        catch( cv::Exception& e)
        {
            const QString err_msg = QString::fromUtf8(e.what());
            qDebug() << "Exception caught:" << err_msg;
        }
        m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, selectedColorFormat));
    }
}

void QtObjectDetector::applyThreshold(double threshold, double thresholdMax, Base::e_OpenCVThresholdType type)
{
    const Base::e_OpenCVColorFormat currentColorFormat = m_pPipelineHandler->getImagePipeline().back().second;
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getThreshold()(m_pPipelineHandler->getImagePipeline().back().first, newImage, threshold, thresholdMax, type);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyAdaptiveThreshold(double maxValue, Base::e_OpenCVAdaptivThresholdMethod adaptiveMethod, Base::e_OpenCVAdaptiveThresholdType thresholdType, Base::e_OpenCVBlockSize blockSize, double C)
{
    const Base::e_OpenCVColorFormat currentColorFormat = m_pPipelineHandler->getImagePipeline().back().second;
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getAdaptiveThreshold()(m_pPipelineHandler->getImagePipeline().back().first, newImage, maxValue, adaptiveMethod, thresholdType, blockSize, C);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyMedianBlur(Base::e_OpenCVKSize ksize)
{
    const Base::e_OpenCVColorFormat currentColorFormat = m_pPipelineHandler->getImagePipeline().back().second;
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getMedianBlur()(m_pPipelineHandler->getImagePipeline().back().first, newImage, ksize);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyBlur(cv::Size ksize, cv::Point anchor, Base::e_OPenCVBorderType borderType)
{
    const Base::e_OpenCVColorFormat currentColorFormat = m_pPipelineHandler->getImagePipeline().back().second;
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getBlur()(m_pPipelineHandler->getImagePipeline().back().first, newImage, ksize, anchor, borderType);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyGaussianBlur(cv::Size ksize, double sigmaX, double sigmaY, Base::e_OPenCVBorderType borderType)
{
    const Base::e_OpenCVColorFormat currentColorFormat = m_pPipelineHandler->getImagePipeline().back().second;
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getGaussianBlur()(m_pPipelineHandler->getImagePipeline().back().first, newImage, ksize, sigmaX, sigmaY, borderType);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyBitwiseNot()
{
    const Base::e_OpenCVColorFormat currentColorFormat = m_pPipelineHandler->getImagePipeline().back().second;
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getBitwiseNot()(m_pPipelineHandler->getImagePipeline().back().first, newImage, cv::noArray());
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyPow(double power)
{
    const Base::e_OpenCVColorFormat currentColorFormat = m_pPipelineHandler->getImagePipeline().back().second;
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getPow()(m_pPipelineHandler->getImagePipeline().back().first, power, newImage);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyErode(cv::Mat kernel, cv::Point anchor, int iterations, Base::e_OPenCVBorderType borderType, const cv::Scalar &borderValue)
{
    const Base::e_OpenCVColorFormat currentColorFormat = m_pPipelineHandler->getImagePipeline().back().second;
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getErode()(m_pPipelineHandler->getImagePipeline().back().first, newImage, kernel, anchor, iterations, borderType, borderValue);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applySobel(int ddepth, int dx, int dy, Base::e_OpenCVKSizeExt ksize, double scale, double delta, Base::e_OPenCVBorderType borderType)
{
    const Base::e_OpenCVColorFormat currentColorFormat = m_pPipelineHandler->getImagePipeline().back().second;
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getSobel()(m_pPipelineHandler->getImagePipeline().back().first, newImage, ddepth, dx, dy, ksize, scale, delta, borderType);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyLaplacian(int ddepth, Base::e_OpenCVKSizeExt ksize, double scale, double delta, Base::e_OPenCVBorderType borderType)
{
    const Base::e_OpenCVColorFormat currentColorFormat = m_pPipelineHandler->getImagePipeline().back().second;
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getLaplacian()(m_pPipelineHandler->getImagePipeline().back().first, newImage, ddepth, ksize, scale, delta, borderType);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyAddWeighted(double alphaSRC1, double betaSRC2, double gamma, int dtype)
{
    const Base::e_OpenCVColorFormat currentColorFormat = m_pPipelineHandler->getImagePipeline().back().second;

    auto currentPipelineSize = m_pPipelineHandler->getImagePipeline().size();
    cv::Mat forelastImage = m_pPipelineHandler->getImagePipeline().at(currentPipelineSize-2).first;
    cv::Mat outputArray;

    try{
        m_pPipelineHandler->getAddWeighted()(m_pPipelineHandler->getImagePipeline().back().first, alphaSRC1, forelastImage, betaSRC2, gamma, outputArray, dtype);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(outputArray, currentColorFormat));
}

void QtObjectDetector::on_pushButton_AddToPipeline_clicked()
{
    m_pPipelineHandler->getFunctionPipeline().push_back(m_lastFunction);
    ui->comboBox_PipelineStepSelector->setEnabled(true);
    ui->comboBox_PipelineStepSelector->addItem(m_lastFunctionString);
    ui->comboBox_PipelineStepSelector->setCurrentIndex(ui->comboBox_PipelineStepSelector->findText(m_lastFunctionString));
    ui->pushButton_AddToPipeline->setEnabled(false);
    ui->pushButton_DeleteFromPipeline->setEnabled(true);
    ui->lineEdit_PipelineName->setEnabled(true);

    if(ui->lineEdit_PipelineName->text().length() > 0 && ui->lineEdit_PipelineName->text().at(0).isLetter())
    {
        ui->pushButton_SavePipeline->setEnabled(true);
    }
}

void QtObjectDetector::on_pushButton_DeleteFromPipeline_clicked()
{
    const auto currentIndex = ui->comboBox_PipelineStepSelector->currentIndex();
    ui->comboBox_PipelineStepSelector->removeItem(currentIndex);
    if(static_cast<int>(m_pPipelineHandler->getFunctionPipeline().size()) > currentIndex)
    {
        m_pPipelineHandler->getFunctionPipeline().erase(m_pPipelineHandler->getFunctionPipeline().begin() + currentIndex);
        m_pPipelineHandler->getImagePipeline().erase(m_pPipelineHandler->getImagePipeline().begin() + currentIndex);
    }
    if(m_pPipelineHandler->getFunctionPipeline().empty())
    {
        ui->comboBox_PipelineStepSelector->setEnabled(false);
        ui->pushButton_DeleteFromPipeline->setEnabled(false);
        ui->pushButton_ApplyPipeline->setEnabled(false);
        ui->pushButton_SavePipeline->setEnabled(false);
        ui->lineEdit_PipelineName->setEnabled(false);
    }
}

void QtObjectDetector::on_pushButton_SavePipeline_clicked()
{
    const auto pipelineName = ui->lineEdit_PipelineName->text();
    auto x = m_pPipelineHandler->getFunctionPipeline();
    if(pipelineName.at(0).isLetter() && !m_pPipelineHandler->getFunctionPipeline().empty())
    {
        const auto pipelinePair = std::pair<std::vector<std::function<void()>>, QString>(m_pPipelineHandler->getFunctionPipeline(), pipelineName);
        m_pPipelineHandler->getAvailablePipelines().push_back(pipelinePair);
        ui->comboBox_PipelineNameSelector->setEnabled(true);
        ui->comboBox_PipelineNameSelectorVideo->setEnabled(true);
        ui->pushButton_ApplyPipeline->setEnabled(true);
        ui->pushButton_ApplyPipelineVideo->setEnabled(true);
        ui->pushButton_DeletePipeline->setEnabled(true);
        ui->pushButton_DeletePipelineVideo->setEnabled(true);
        ui->comboBox_PipelineNameSelector->addItem(pipelineName);
        ui->comboBox_PipelineNameSelectorVideo->addItem(pipelineName);
    }
}

void QtObjectDetector::on_pushButton_DeletePipeline_clicked()
{
    const int currentIndex = ui->comboBox_PipelineNameSelector->currentIndex();
    if(currentIndex >= 0)
    {
        ui->comboBox_PipelineNameSelector->removeItem(currentIndex);
        ui->comboBox_PipelineNameSelectorVideo->removeItem(currentIndex);
        if (static_cast<int>(m_pPipelineHandler->getAvailablePipelines().size() - 1) >= currentIndex)
        {
            m_pPipelineHandler->getAvailablePipelines().erase(m_pPipelineHandler->getAvailablePipelines().begin() + currentIndex);
        }
        if(m_pPipelineHandler->getAvailablePipelines().size() == 0)
        {
            ui->pushButton_ApplyPipeline->setEnabled(false);
        }
    }
}

void QtObjectDetector::on_lineEdit_PipelineName_textChanged(const QString &arg1)
{
    if(arg1.size() && arg1.at(0).isLetter() && ui->comboBox_PipelineStepSelector->count() > 0)
    {
        ui->pushButton_SavePipeline->setEnabled(true);
    }
    else
    {
        ui->pushButton_SavePipeline->setEnabled(false);
    }
}

void QtObjectDetector::on_pushButton_ApplyPipeline_clicked()
{
    const QString selectedPipeline = ui->comboBox_PipelineNameSelector->currentText();
    for (const auto& pipeline : m_pPipelineHandler->getAvailablePipelines())
    {
        if(pipeline.second == selectedPipeline)
        {
            const auto functionPipeline = pipeline.first;
            for(const auto& function : functionPipeline)
            {
                function();
            }
            break;
        }
    }
    if(m_pPipelineHandler->getImagePipeline().size() > 0)
    {
        loadImageToQLabel(m_pPipelineHandler->getImagePipeline().size()-1);
    }
}

void QtObjectDetector::on_comboBox_FunctionSelector_currentIndexChanged(const QString &arg1)
{
    QLayout* myLayout = ui->widget_Arguments->layout();
    if(myLayout != nullptr)
    {
        QLayoutItem* item;
        while((item = myLayout->takeAt(0)) != nullptr)
        {
            delete item->widget();
            myLayout->removeItem(item);
        }
        delete item;
        delete myLayout;
    }

    QVBoxLayout *pLayout = new QVBoxLayout();
    LayoutHandler::createLayout(pLayout, arg1);
    if(pLayout != nullptr)
    {
        pLayout->setAlignment(Qt::AlignTop);
        ui->widget_Arguments->setLayout(pLayout);
    }
}

void QtObjectDetector::on_comboBox_PipelineNameSelector_currentIndexChanged(int index)
{
    if(ui->comboBox_PipelineNameSelectorVideo->currentIndex() != index)
    {
        ui->comboBox_PipelineNameSelectorVideo->setCurrentIndex(index);
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
    if(m_pThreadVideo && m_pThreadVideo->isRunning())
    {
        m_runThread = false;
        return;
    }
    else{
        m_runThread = true;
    }

    setUpVideo();
    const auto filepath = m_pVideoLoader->getFileInfo().absoluteFilePath().toUtf8();
    if (!m_pInputVideo->open(filepath.data(), cv::CAP_ANY)) return;

    std::function runVideo = [this](){

        const double fps = m_pInputVideo.get()->get(cv::CAP_PROP_FPS);
        qDebug() << fps << "Fps";
        int frameCounter = 0;

        for(;;)
        {
            if(!m_pInputVideo->isOpened())
            {
                qDebug() << "Video is not opened anymore, stop processing";
                break;
            }
            m_pPipelineHandler->getImagePipeline().clear();
            qDebug() << "Video Frame: " << frameCounter;
            m_pInputVideo->read(m_pOutputVideoImage);
            if (!m_pOutputVideoImage.empty())
            {
                qDebug() << m_pPipelineHandler->getImagePipeline().size();
                qDebug() << "Video Image Format before filter: " << QString::fromStdString(cv::typeToString(m_pOutputVideoImage.type()));
                m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(m_pOutputVideoImage, getColorFormat(m_pOutputVideoImage)));
            }
            else
            {
                qDebug() << "Empty Image, end video";
                break;
            }
            if(!m_selectedVideoPipeline.empty())
            {
                for(const auto& function : m_selectedVideoPipeline)
                {
                    function();
                }
                if(m_pPipelineHandler->getImagePipeline().size() > 0)
                {
                    qDebug() << m_pPipelineHandler->getImagePipeline().size();
                    m_pOutputVideoImage = m_pPipelineHandler->getImagePipeline().back().first;
                    qDebug() << "Video Image Format post filter: " << QString::fromStdString(cv::typeToString(m_pOutputVideoImage.type()));
                }
            }

            emit newVideoImage();
            QThread::msleep(1000/static_cast<uint>(fps));
            frameCounter++;
            if(!m_runThread) break;
        }
    };

    m_pThreadVideo = QThread::create(runVideo);
    m_pThreadVideo->setObjectName("runVideo");
    connect(m_pThreadVideo, &QThread::started, this, &QtObjectDetector::setStopVideoSettings);
    connect(m_pThreadVideo, &QThread::finished, this, &QtObjectDetector::setStartVideoSettings);
    m_pThreadVideo->start();
}

void QtObjectDetector::on_newVideoImage()
{
    QImage qimg((m_pOutputVideoImage).data, (m_pOutputVideoImage).cols, (m_pOutputVideoImage).rows, static_cast<int>((m_pOutputVideoImage).step), static_cast<QImage::Format>(getColorFormat(m_pOutputVideoImage)));
    if(m_pVideoImageItem)
    {
        m_pVideoImageItem->setPixmap(QPixmap::fromImage(qimg.rgbSwapped()));
    }
}

void QtObjectDetector::setUpVideo()
{
    if(m_pVideoImageItem == nullptr)
    {
        m_pVideoImageItem = new QGraphicsPixmapItem();
        ui->graphicsView_VideoLoader->setScene(m_pScene);
        m_pScene->addItem(m_pVideoImageItem);
    }
    if(!m_pInputVideo)
    {
        m_pInputVideo = std::make_unique<cv::VideoCapture>();
    }
    if(!m_pInputCam)
    {
        m_pInputCam = std::make_unique<cv::VideoCapture>();
    }
    //todo delete
}

//When camera is running
void QtObjectDetector::setStopCameraSettings() const
{
    ui->pushButton_StartCamera->setText("Stop Camera");
    ui->comboBox_LiveCamera->setEnabled(false);
    ui->checkBox_LiveCamera->setEnabled(false);
}

//When camera is not running
void QtObjectDetector::setStartCameraSettings() const
{
    if(m_pInputCam && m_pInputCam->isOpened())
    {
        m_pInputCam->release();
    }
    ui->pushButton_StartCamera->setText("Start Camera");
    ui->comboBox_LiveCamera->setEnabled(true);
    ui->checkBox_LiveCamera->setEnabled(true);
}

//When video is running
void QtObjectDetector::setStopVideoSettings() const
{
    ui->checkBox_LiveCamera->setEnabled(false);
    ui->pushButton_SelectVideo->setEnabled(false);
    ui->pushButton_LoadVideo->setText("Stop Video");
}

//When video is not running
void QtObjectDetector::setStartVideoSettings() const
{
    if(m_pInputVideo && m_pInputVideo->isOpened())
    {
        m_pInputVideo->release();
    }
    ui->checkBox_LiveCamera->setEnabled(true);
    ui->pushButton_SelectVideo->setEnabled(true);
    ui->pushButton_LoadVideo->setText("Start Video");
}

void QtObjectDetector::on_pushButton_StartCamera_clicked()
{
    if(m_pThreadVideo && m_pThreadVideo->isRunning())
    {
        m_runThread = false;
        return;
    }
    else{
        m_runThread = true;
    }

    setUpVideo();
    const int currentDeviceIndex = ui->comboBox_LiveCamera->currentIndex();
    if(!m_Cameras.empty())
    {
        m_pInputCam->open(currentDeviceIndex, cv::CAP_ANY);
    }
    if(!m_pInputCam->isOpened())
    {
        qDebug() << "ERROR! Unable to open camera";
        return;
    }

    std::function runCam = [&](){

        const double fps = m_pInputCam.get()->get(cv::CAP_PROP_FPS);
        qDebug() << fps << "Fps";
        int frameCounter = 0;

        for (;;)
        {
            if(!m_pInputCam->isOpened())
            {
                qDebug() << "Camera is not opened anymore, stop processing";
                break;
            }
            m_pPipelineHandler->getImagePipeline().clear();
            qDebug() << "Video Frame: " << frameCounter;

            m_pInputCam->read(m_pOutputVideoImage);
            if (!m_pOutputVideoImage.empty()) {
                qDebug() << m_pPipelineHandler->getImagePipeline().size();
                qDebug() << "Video Image Format before filter: " << QString::fromStdString(cv::typeToString(m_pOutputVideoImage.type()));
                m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(m_pOutputVideoImage, getColorFormat(m_pOutputVideoImage)));

            }
            else
            {
                qDebug() << "Empty Image, end video";
                break;
            }
            if(!m_selectedVideoPipeline.empty())
            {
                for(const auto& function : m_selectedVideoPipeline)
                {
                    function();
                }
                if(m_pPipelineHandler->getImagePipeline().size() > 0)
                {
                    qDebug() << m_pPipelineHandler->getImagePipeline().size();
                    m_pOutputVideoImage = m_pPipelineHandler->getImagePipeline().back().first;
                    qDebug() << "Video Image Format post filter: " << QString::fromStdString(cv::typeToString(m_pOutputVideoImage.type()));
                }
            }
            emit newVideoImage();
            frameCounter++;
            if(!m_runThread) break;
        }
    };

    QThread *thread = QThread::create(runCam);
    thread->setObjectName("runCam");
    connect(thread, &QThread::started, this, &QtObjectDetector::setStopCameraSettings);
    connect(thread, &QThread::finished, this, &QtObjectDetector::setStartCameraSettings);
    thread->start();
}

void QtObjectDetector::on_checkBox_LiveCamera_stateChanged(int arg1)
{
    if(static_cast<bool>(arg1) == true)
    {
        ui->pushButton_StartCamera->setEnabled(true);
        ui->comboBox_LiveCamera->setEnabled(true);
        ui->pushButton_SelectVideo->setEnabled(false);
        ui->pushButton_LoadVideo->setEnabled(false);
    }
    else
    {
        ui->pushButton_StartCamera->setEnabled(false);
        ui->comboBox_LiveCamera->setEnabled(false);
        ui->pushButton_SelectVideo->setEnabled(true);

        if(ui->label_videoName->text().length() >= 3 && ui->label_videoName->text().contains("."))
        {
            ui->pushButton_LoadVideo->setEnabled(true);
        }
    }
}

void QtObjectDetector::on_pushButton_ApplyPipelineVideo_clicked()
{
    const QString selectedPipeline = ui->comboBox_PipelineNameSelectorVideo->currentText();
    for (const auto& pipeline : m_pPipelineHandler->getAvailablePipelines())
    {
        if(pipeline.second == selectedPipeline)
        {
            m_selectedVideoPipeline = pipeline.first;
            break;
        }
    }
}

void QtObjectDetector::on_pushButton_DeletePipelineVideo_clicked()
{
    on_pushButton_DeletePipeline_clicked();
}

void QtObjectDetector::on_comboBox_PipelineNameSelectorVideo_currentIndexChanged(int index)
{
    if(ui->comboBox_PipelineNameSelector->currentIndex() != index)
    {
        ui->comboBox_PipelineNameSelector->setCurrentIndex(index);
    }
}
