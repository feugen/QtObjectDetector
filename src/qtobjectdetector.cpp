#include "qtobjectdetector.h"
#include "ui_qtobjectdetector.h"
#include "layouthandler.h"

#include <QPixmap>
#include <QGraphicsVideoItem>
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

    QStringList functionsTypeList;
    const QMetaEnum metaEnumTypes = QMetaEnum::fromType<Base::e_OPenCVFunctionType>();
    for (int i = 0; i < metaEnumTypes.keyCount() ; i++ )
    {
        const QString functionsType = metaEnumTypes.valueToKey(i);
        const QChar firstLetter = functionsType.at(0).toUpper();
        functionsTypeList += functionsType.toLower().replace(0, 1, firstLetter);
    }
    ui->comboBox_FunctionTypeSelector->addItems(functionsTypeList);

    connect(m_pImageDialog.get(), &QFileDialog::fileSelected, this, &QtObjectDetector::on_fileSelected);
    connect(m_pVideoDialog.get(), &QFileDialog::fileSelected, this, &QtObjectDetector::on_videoSelected);
    connect(this, &QtObjectDetector::newVideoImage , this, &QtObjectDetector::on_newVideoImage);
    connect(ui->actionExit, &QAction::triggered, this, &QApplication::exit);
}

QtObjectDetector::~QtObjectDetector()
{
    deleteItems();
    delete ui;
}

//General Part


void QtObjectDetector::on_pushButton_ApplyFunction_clicked()
{
    if (!m_pPipelineHandler->getImagePipeline().empty())
    {
        Base::e_OpenCVFunction selectedFunction = Base::e_OpenCVFunction::CvtColor; //Default initialization
        const auto selectedFunctionName = ui->comboBox_FunctionSelector->currentText();

        const QMetaEnum metaEnumTypes = QMetaEnum::fromType<Base::e_OpenCVFunction>();
        for (int i = 0; i < metaEnumTypes.keyCount() ; i++ )
        {
            const QString functionsName = metaEnumTypes.valueToKey(i);
            if(functionsName == selectedFunctionName)
            {
                selectedFunction = static_cast<Base::e_OpenCVFunction>(i);
                break;
            }
            if(i == metaEnumTypes.keyCount()-1)
            {
                //No implemented function found, dont process
                return;
            }
        }

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

                cv::Mat element = cv::getStructuringElement(arg3Value, cv::Size( 2*arg4Value + 1, 2*arg4Value+1 ), cv::Point( arg4Value, arg4Value));
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
        case Base::e_OpenCVFunction::Canny:
        {
            const auto arg1 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("doubleSpinBoxThreshold1");
            const auto arg2 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("doubleSpinBoxThreshold2");
            const auto arg3 = ui->widget_Arguments->findChild<QComboBox*>("comboboxApertureSize");
            const auto arg4 = ui->widget_Arguments->findChild<QComboBox*>("comboboxL2gradient");

            if(arg1 && arg2 && arg3 && arg4)
            {
                const auto arg1Value = arg1->value();
                const auto arg2Value = arg2->value();
                const auto arg3Value = arg3->currentIndex();
                const auto arg3ValueInt = 2*arg3Value+3;
                const auto arg3ValueText = arg3->currentText();
                const auto arg4Value = arg4->currentIndex();

                //Verify data
                const auto enumValue3 = static_cast<Base::e_OpenCVKSize>(arg3ValueInt);
                assert(Base::QEnumToQString(enumValue3) == arg3ValueText);

                m_lastFunction = [=](){applyCanny(arg1Value, arg2Value, enumValue3, arg4Value);};
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
        case Base::e_OpenCVFunction::CascadeClassifier:
        {
            const auto arg1 = ui->widget_Arguments->findChild<QComboBox*>("comboboxCascade");

            if(arg1)
            {
                const auto arg1Value = arg1->currentText();

                m_lastFunction = [=](){applyCascadeClassifier(arg1Value.toStdString());};
            }
            break;
        }
        case Base::e_OpenCVFunction::ShiTomasi:
        {
            const auto arg1 = ui->widget_Arguments->findChild<QSpinBox*>("spinBoxMaxCorners");
            const auto arg2 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("spinBoxQualityLevel");
            const auto arg3 = ui->widget_Arguments->findChild<QDoubleSpinBox*>("spinBoxMinDistance");
            const auto arg4 = ui->widget_Arguments->findChild<QComboBox*>("comboboxBlockSize");

            if(arg1 && arg2 && arg3 && arg4)
            {
                const auto arg1Value = arg1->value();
                const auto arg2Value = arg2->value();
                const auto arg3Value = arg3->value();
                const auto arg4Value = arg4->currentIndex();
                const auto arg4ValueInt = 2*arg4Value+3;
                const auto arg4ValueText = arg4->currentText();

                //Verify data
                const auto enumValue4 = static_cast<Base::e_OpenCVKSize>(arg4ValueInt);
                assert(Base::QEnumToQString(enumValue4).mid(6) == arg4ValueText);

                m_lastFunction = [=](){applyShiTomasi(arg1Value, arg2Value, arg3Value, enumValue4);};
            }
            break;
        }

        case Base::e_OpenCVFunction::BackgroundSubtraction:
        {
            const auto arg1 = ui->widget_Arguments->findChild<QComboBox*>("comboboxSubtractionMethod");

            if(arg1)
            {
                const auto arg1Value = arg1->currentIndex();
                const auto arg1ValueText = arg1->currentText();

                //Verify data
                const auto enumValue1 = static_cast<Base::e_OpenCVBackgroundSubtractor>(arg1Value);
                assert(Base::QEnumToQString(enumValue1) == arg1ValueText);

                m_lastFunction = [=](){applyBackgroundSubtractor(enumValue1);};
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
            ui->pushButton_UndoFunction->setEnabled(true);
            ui->pushButton_ApplyFunction->setEnabled(false);
            ui->comboBox_FunctionTypeSelector->setEnabled(false);
            ui->comboBox_FunctionSelector->setEnabled(false);
            ui->widget_Arguments->setEnabled(false);
        }
    }
}

void QtObjectDetector::on_pushButton_UndoFunction_clicked()
{
    const auto selectedFunctionName = ui->comboBox_FunctionSelector->currentText();
    if(m_lastFunctionString == selectedFunctionName)
    {
        if(m_pPipelineHandler->getImagePipeline().size() > 1)
        {
            m_pPipelineHandler->getImagePipeline().pop_back();
            if(m_pPipelineHandler->getImagePipeline().size() == 1)
            {
                ui->pushButton_UndoFunction->setEnabled(false);
                ui->pushButton_ApplyFunction->setEnabled(true);
                ui->comboBox_FunctionTypeSelector->setEnabled(true);
                ui->comboBox_FunctionSelector->setEnabled(true);
                ui->widget_Arguments->setEnabled(true);
                ui->pushButton_AddToPipeline->setEnabled(false);
            }
        }
        loadImageToQLabel(m_pPipelineHandler->getImagePipeline().size() - 1);
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
    QStringList mimeTypeFilters({"image/jpeg", // will show "JPEG image (*.jpeg *.jpg *.jpe)
                                 "image/png",  // will show "PNG image (*.png)"
                                 "image/bmp"  // will show "BMP image (*.bmp)"
                                });
    m_pImageDialog->setMimeTypeFilters(mimeTypeFilters);
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
        m_imageSizePolicy.autoSize = ui->checkBox_autoSize->isChecked();
        m_imageSizePolicy.autoScaled = ui->checkBox_autoScale->isChecked();
        m_imageSizePolicy.x = m_imageSizePolicy.autoSize ? m_pPipelineHandler->getImagePipeline().at(0).first.cols : (ui->xSpinBox->value() > m_pPipelineHandler->getImagePipeline().at(0).first.cols ? m_pPipelineHandler->getImagePipeline().at(0).first.cols : ui->xSpinBox->value());
        m_imageSizePolicy.y = m_imageSizePolicy.autoSize  ? m_pPipelineHandler->getImagePipeline().at(0).first.rows : (ui->ySpinBox->value() > m_pPipelineHandler->getImagePipeline().at(0).first.rows ? m_pPipelineHandler->getImagePipeline().at(0).first.rows : ui->ySpinBox->value());
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
    QFile file(m_pPhotoLoader->getFileInfo().absoluteFilePath().toUtf8());
    if (!file.open(QFile::ReadOnly)) return;
    if (!m_pPipelineHandler->getImagePipeline().size()) return;

    if((m_pPipelineHandler->getImagePipeline().size() - 1) >= storagePosition)
    {
        qDebug() << "Image Format loaded: " << QString::fromStdString(cv::typeToString(m_pPipelineHandler->getImagePipeline().at(storagePosition).first.type()));
        const std::unique_ptr<QImage> imgIn = std::make_unique<QImage>(static_cast<uchar*>(m_pPipelineHandler->getImagePipeline().at(storagePosition).first.data), m_imageSizePolicy.x, m_imageSizePolicy.y, static_cast<int>(m_pPipelineHandler->getImagePipeline().at(storagePosition).first.step), static_cast<QImage::Format>(m_pPipelineHandler->getImagePipeline().at(storagePosition).second));
        QPixmap myPixmap;
        if(ui->checkBox_autoSize->isChecked())
        {
            const int lineWidth = ui->qLabel_PhotoLoader->lineWidth(); // we need to substract the line width.
            const int bugFix = 1 + 4*lineWidth; //Where does the "1" come from??? Without a box around qlabel (with line width = 1), the 1 fixes the scaling. Why is the Line-width *2 wrong? Fixes needed
            myPixmap = QPixmap::fromImage(*imgIn.get()).scaled(ui->qLabel_PhotoLoader->size().width() - bugFix, ui->qLabel_PhotoLoader->size().height() - bugFix, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        else{
            myPixmap = QPixmap::fromImage(*imgIn.get()).scaled(ui->xSpinBox->value(),ui->ySpinBox->value(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
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

void QtObjectDetector::on_comboBox_FunctionTypeSelector_currentIndexChanged(const QString &arg1)
{
    QStringList functionsListPart;
    if(arg1 == QStringLiteral("Filtering"))
    {
        const QMetaEnum metaEnumFunctions = QMetaEnum::fromType<Base::e_OpenCVFilteringFunction>();
        for (int i = 0; i < metaEnumFunctions.keyCount() ; i++ )
        {
            functionsListPart += metaEnumFunctions.valueToKey(i);
        }
    }
    else if(arg1 == QStringLiteral("Transformation"))
    {
        const QMetaEnum metaEnumFunctions = QMetaEnum::fromType<Base::e_OpenCVTransformationFunction>();
        for (int i = 0; i < metaEnumFunctions.keyCount() ; i++ )
        {
            functionsListPart += metaEnumFunctions.valueToKey(i);
        }
    }
    else if(arg1 == QStringLiteral("Detection"))
    {
        const QMetaEnum metaEnumFunctions = QMetaEnum::fromType<Base::e_OpenCVDetectionFunction>();
        for (int i = 0; i < metaEnumFunctions.keyCount() ; i++ )
        {
            functionsListPart += metaEnumFunctions.valueToKey(i);
        }
    }
    else if(arg1 == QStringLiteral("Tracking"))
    {
        const QMetaEnum metaEnumFunctions = QMetaEnum::fromType<Base::e_OpenCVTrackingFunction>();
        for (int i = 0; i < metaEnumFunctions.keyCount() ; i++ )
        {
            functionsListPart += metaEnumFunctions.valueToKey(i);
        }
    }
    ui->comboBox_FunctionSelector->clear();
    ui->comboBox_FunctionSelector->addItems(functionsListPart);
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
        const Base::e_OpenCVColorFormat newColorFormat = newImage.channels() == 3 ? Base::e_OpenCVColorFormat::COLOR : Base::e_OpenCVColorFormat::GRAY;
        m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, newColorFormat));
    }
}

void QtObjectDetector::applyThreshold(double threshold, double thresholdMax, Base::e_OpenCVThresholdType type)
{
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getThreshold()(m_pPipelineHandler->getImagePipeline().back().first, newImage, threshold, thresholdMax, static_cast<int>(type));
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    const Base::e_OpenCVColorFormat currentColorFormat = newImage.channels() == 3 ? Base::e_OpenCVColorFormat::COLOR : Base::e_OpenCVColorFormat::GRAY;
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyAdaptiveThreshold(double maxValue, Base::e_OpenCVAdaptivThresholdMethod adaptiveMethod, Base::e_OpenCVAdaptiveThresholdType thresholdType, Base::e_OpenCVBlockSize blockSize, double C)
{
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getAdaptiveThreshold()(m_pPipelineHandler->getImagePipeline().back().first, newImage, maxValue, static_cast<int>(adaptiveMethod), static_cast<int>(thresholdType), static_cast<int>(blockSize), C);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    const Base::e_OpenCVColorFormat currentColorFormat = newImage.channels() == 3 ? Base::e_OpenCVColorFormat::COLOR : Base::e_OpenCVColorFormat::GRAY;
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyMedianBlur(Base::e_OpenCVKSize ksize)
{
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getMedianBlur()(m_pPipelineHandler->getImagePipeline().back().first, newImage, static_cast<int>(ksize));
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    const Base::e_OpenCVColorFormat currentColorFormat = newImage.channels() == 3 ? Base::e_OpenCVColorFormat::COLOR : Base::e_OpenCVColorFormat::GRAY;
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyBlur(cv::Size ksize, cv::Point anchor, Base::e_OPenCVBorderType borderType)
{
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getBlur()(m_pPipelineHandler->getImagePipeline().back().first, newImage, ksize, anchor, static_cast<int>(borderType));
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    const Base::e_OpenCVColorFormat currentColorFormat = newImage.channels() == 3 ? Base::e_OpenCVColorFormat::COLOR : Base::e_OpenCVColorFormat::GRAY;
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyGaussianBlur(cv::Size ksize, double sigmaX, double sigmaY, Base::e_OPenCVBorderType borderType)
{
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getGaussianBlur()(m_pPipelineHandler->getImagePipeline().back().first, newImage, ksize, sigmaX, sigmaY, static_cast<int>(borderType));
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    const Base::e_OpenCVColorFormat currentColorFormat = newImage.channels() == 3 ? Base::e_OpenCVColorFormat::COLOR : Base::e_OpenCVColorFormat::GRAY;
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyBitwiseNot()
{
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getBitwiseNot()(m_pPipelineHandler->getImagePipeline().back().first, newImage, cv::noArray());
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    const Base::e_OpenCVColorFormat currentColorFormat = newImage.channels() == 3 ? Base::e_OpenCVColorFormat::COLOR : Base::e_OpenCVColorFormat::GRAY;
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyPow(double power)
{
    cv::Mat convertedImage;
    cv::Mat newImage;
    cv::Mat finalImage;

    const auto depth = m_pPipelineHandler->getImagePipeline().back().first.depth();

    if(!(depth == CV_32F || depth == CV_64F))
    {
        m_pPipelineHandler->getImagePipeline().back().first.convertTo(convertedImage, CV_32F);
    }
    else
    {
        m_pPipelineHandler->getImagePipeline().back().first.copyTo(convertedImage);
    }

    try{
        m_pPipelineHandler->getPow()(convertedImage, power, newImage);
        newImage.convertTo(finalImage, 0);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    const Base::e_OpenCVColorFormat currentColorFormat = finalImage.channels() == 3 ? Base::e_OpenCVColorFormat::COLOR : Base::e_OpenCVColorFormat::GRAY;
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(finalImage, currentColorFormat));
}

void QtObjectDetector::applyErode(cv::Mat kernel, cv::Point anchor, int iterations, Base::e_OPenCVBorderType borderType, const cv::Scalar &borderValue)
{
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getErode()(m_pPipelineHandler->getImagePipeline().back().first, newImage, kernel, anchor, iterations, static_cast<int>(borderType), borderValue);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    const Base::e_OpenCVColorFormat currentColorFormat = newImage.channels() == 3 ? Base::e_OpenCVColorFormat::COLOR : Base::e_OpenCVColorFormat::GRAY;
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applySobel(int ddepth, int dx, int dy, Base::e_OpenCVKSizeExt ksize, double scale, double delta, Base::e_OPenCVBorderType borderType)
{
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getSobel()(m_pPipelineHandler->getImagePipeline().back().first, newImage, ddepth, dx, dy, static_cast<int>(ksize), scale, delta, static_cast<int>(borderType));
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    const Base::e_OpenCVColorFormat currentColorFormat = newImage.channels() == 3 ? Base::e_OpenCVColorFormat::COLOR : Base::e_OpenCVColorFormat::GRAY;
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyLaplacian(int ddepth, Base::e_OpenCVKSizeExt ksize, double scale, double delta, Base::e_OPenCVBorderType borderType)
{
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getLaplacian()(m_pPipelineHandler->getImagePipeline().back().first, newImage, ddepth, static_cast<int>(ksize), scale, delta, static_cast<int>(borderType));
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    const Base::e_OpenCVColorFormat currentColorFormat = newImage.channels() == 3 ? Base::e_OpenCVColorFormat::COLOR : Base::e_OpenCVColorFormat::GRAY;
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyCanny(double threshold1, double threshold2, Base::e_OpenCVKSize ksize, bool L2gradient)
{
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getCanny()(m_pPipelineHandler->getImagePipeline().back().first, newImage, threshold1, threshold2, static_cast<int>(ksize), L2gradient);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    const Base::e_OpenCVColorFormat currentColorFormat = newImage.channels() == 3 ? Base::e_OpenCVColorFormat::COLOR : Base::e_OpenCVColorFormat::GRAY;
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyAddWeighted(double alphaSRC1, double betaSRC2, double gamma, int dtype)
{
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
    const Base::e_OpenCVColorFormat currentColorFormat = outputArray.channels() == 3 ? Base::e_OpenCVColorFormat::COLOR : Base::e_OpenCVColorFormat::GRAY;
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(outputArray, currentColorFormat));
}

void QtObjectDetector::applyCascadeClassifier(const cv::String &fileName)
{
    cv::CascadeClassifier cascade;
    if(!cascade.load("data/haarcascades/" + fileName))
    {
        qDebug() << "Error loading cascade " + QString::fromStdString(fileName);
        return;
    }
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getCascadeClassifier()(m_pPipelineHandler->getImagePipeline().back().first, newImage, cascade);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    const Base::e_OpenCVColorFormat currentColorFormat = newImage.channels() == 3 ? Base::e_OpenCVColorFormat::COLOR : Base::e_OpenCVColorFormat::GRAY;
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));
}

void QtObjectDetector::applyShiTomasi(int maxCorners, double qualityLevel, double minDistance, Base::e_OpenCVKSize ksize)
{
    cv::Mat tempImage;
    std::vector<cv::Point2f> corners;

    const int channels = m_pPipelineHandler->getImagePipeline().back().first.channels();
    if(channels == 3)
    {
        cv::cvtColor(m_pPipelineHandler->getImagePipeline().back().first, tempImage, cv::COLOR_BGR2GRAY );
    }
    else if(channels == 1)
    {
        m_pPipelineHandler->getImagePipeline().back().first.copyTo(tempImage);
    }

    try{
        m_pPipelineHandler->getShiTomasi()(tempImage, corners, maxCorners, qualityLevel, minDistance, cv::noArray(), static_cast<int>(ksize), false, 0.04);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    qDebug() << "Number of corners detected: " << corners.size();
    const int radius = 10;
    const cv::Scalar color(0,255,0,255);
    for(const auto &corner : corners)
    {
        cv::circle(m_pPipelineHandler->getImagePipeline().back().first, corner, radius, color, 1, cv::FILLED);
    }

    const Base::e_OpenCVColorFormat currentColorFormat = m_pPipelineHandler->getImagePipeline().back().first.channels() == 3 ? Base::e_OpenCVColorFormat::COLOR : Base::e_OpenCVColorFormat::GRAY;
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(m_pPipelineHandler->getImagePipeline().back().first, currentColorFormat));
}

void QtObjectDetector::applyBackgroundSubtractor(const Base::e_OpenCVBackgroundSubtractor subtractor)
{
    cv::Mat newImage;
    try{
        m_pPipelineHandler->getBackgroundSubtraction()(m_pPipelineHandler->getImagePipeline().back().first, newImage, subtractor);
    }
    catch( cv::Exception& e)
    {
        const QString err_msg = QString::fromUtf8(e.what());
        qDebug() << "Exception caught:" << err_msg;
    }
    const Base::e_OpenCVColorFormat currentColorFormat = newImage.channels() == 3 ? Base::e_OpenCVColorFormat::COLOR : Base::e_OpenCVColorFormat::GRAY;
    m_pPipelineHandler->getImagePipeline().push_back(std::pair<cv::Mat, Base::e_OpenCVColorFormat>(newImage, currentColorFormat));

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
    ui->pushButton_UndoFunction->setEnabled(false);
    ui->comboBox_FunctionTypeSelector->setEnabled(true);
    ui->comboBox_FunctionSelector->setEnabled(true);
    ui->widget_Arguments->setEnabled(true);

    if(ui->lineEdit_PipelineName->text().length() > 0 && ui->lineEdit_PipelineName->text().at(0).isLetter())
    {
        ui->pushButton_SavePipeline->setEnabled(true);
    }
}

void QtObjectDetector::on_pushButton_DeleteFromPipeline_clicked()
{
    if (!m_pPipelineHandler->getImagePipeline().size()) return;
    if (!m_pPipelineHandler->getFunctionPipeline().size()) return;

    auto currentIndex = ui->comboBox_PipelineStepSelector->currentIndex();
    if(static_cast<int>(m_pPipelineHandler->getFunctionPipeline().size()) == currentIndex + 1)
    {
        ui->comboBox_PipelineStepSelector->removeItem(currentIndex);
        m_pPipelineHandler->getFunctionPipeline().pop_back();
        m_pPipelineHandler->getImagePipeline().pop_back();

        loadImageToQLabel(m_pPipelineHandler->getImagePipeline().size() - 1);
        m_lastFunctionString = ui->comboBox_PipelineStepSelector->currentText();
        if(m_lastFunctionString == ui->comboBox_FunctionSelector->currentText())
        {
            ui->pushButton_ApplyFunction->setEnabled(true);
        }
    }
    if(m_pPipelineHandler->getFunctionPipeline().empty())
    {
        ui->comboBox_PipelineStepSelector->setEnabled(false);
        ui->pushButton_DeleteFromPipeline->setEnabled(false);
        ui->pushButton_ApplyPipeline->setEnabled(false);
        ui->pushButton_SavePipeline->setEnabled(false);
        ui->lineEdit_PipelineName->setEnabled(false);
        ui->pushButton_ApplyFunction->setEnabled(true);
    }

    currentIndex = ui->comboBox_PipelineStepSelector->currentIndex();
    if (currentIndex == -1) return;
    if((currentIndex + 1) == static_cast<int>(m_pPipelineHandler->getFunctionPipeline().size()))
    {
        ui->pushButton_DeleteFromPipeline->setEnabled(true);
    }
    else{
        ui->pushButton_DeleteFromPipeline->setEnabled(false);
    }
}

void QtObjectDetector::on_comboBox_PipelineStepSelector_currentIndexChanged(int index)
{
    if((index + 1) == static_cast<int>(m_pPipelineHandler->getFunctionPipeline().size()))
    {
        ui->pushButton_DeleteFromPipeline->setEnabled(true);
    }
    else{
        ui->pushButton_DeleteFromPipeline->setEnabled(false);
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

        ui->comboBox_PipelineStepSelector->clear();
        ui->pushButton_DeleteFromPipeline->setEnabled(false);
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
    deleteItems();

    if(m_pLayout) m_pLayout.reset();
    m_pLayout = std::make_unique<QVBoxLayout>();
    LayoutHandler::createLayout(m_pLayout.get(), arg1);
    if(m_pLayout != nullptr)
    {
        m_pLayout->setAlignment(Qt::AlignTop);
        ui->widget_Arguments->setLayout(m_pLayout.get());
    }

    const auto selectedFunctionName = ui->comboBox_FunctionSelector->currentText();
    if(m_lastFunctionString == selectedFunctionName)
    {
        ui->pushButton_UndoFunction->setEnabled(true);
        ui->pushButton_ApplyFunction->setEnabled(false);
    }
    else{
        ui->pushButton_UndoFunction->setEnabled(false);
        ui->pushButton_ApplyFunction->setEnabled(true);
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
    const QStringList mimeTypeFilters({"video/mpeg",
                                       "video/mp4",
                                       "video/ogg",
                                       "video/webm",
                                       "video/x-msvideo"
                                      });
    m_pVideoDialog->setMimeTypeFilters(mimeTypeFilters);
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
    connect(m_pThreadVideo, &QThread::finished, m_pThreadVideo, &QThread::deleteLater);
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
        m_pVideoImageItem = std::make_unique<QGraphicsPixmapItem>();
        ui->graphicsView_VideoLoader->setScene(m_pScene);
        m_pScene->addItem(m_pVideoImageItem.get());
    }
    if(!m_pInputVideo)
    {
        m_pInputVideo = std::make_unique<cv::VideoCapture>();
    }
    if(!m_pInputCam)
    {
        m_pInputCam = std::make_unique<cv::VideoCapture>();
    }
}

void QtObjectDetector::deleteItems()
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
    }
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
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
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
