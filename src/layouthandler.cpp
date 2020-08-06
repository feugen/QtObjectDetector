#include "layouthandler.h"
#include "base.h"
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDir>

LayoutHandler::LayoutHandler(QObject *parent) : QObject(parent)
{

}

void LayoutHandler::createLayout(QLayout *layout, QString function)
{
    if(layout == nullptr) return void();
    if(function == "CvtColor")
    {
        QLabel* labelFormat = new QLabel();
        labelFormat->setText("Color Format:");

        QComboBox* comboboxFormat = new QComboBox();
        comboboxFormat->setObjectName("comboboxFormat");
        comboboxFormat->addItem("Gray");

        layout->addWidget(labelFormat);
        layout->addWidget(comboboxFormat);
    }
    else if(function == "Threshold")
    {
        //Argument 1
        QLabel* labelThresh = new QLabel();
        labelThresh->setText("Threshold Value:");

        QDoubleSpinBox* spinboxTresholdValue = new QDoubleSpinBox();
        spinboxTresholdValue->setObjectName("spinboxTresholdValue");
        spinboxTresholdValue->setRange(0.0, 255);
        spinboxTresholdValue->setDecimals(1);
        spinboxTresholdValue->setValue(155.0);

        //Argument 2
        QLabel* labelThreshMax = new QLabel();
        labelThreshMax->setText("Threshold Max Value:");

        QDoubleSpinBox* spinboxTresholdValueMax = new QDoubleSpinBox();
        spinboxTresholdValueMax->setObjectName("spinboxTresholdValueMax");
        spinboxTresholdValueMax->setRange(0.0, 255);
        spinboxTresholdValueMax->setDecimals(1);
        spinboxTresholdValueMax->setValue(210.0);

        //Argument 3
        QLabel* labelThreshType = new QLabel();
        labelThreshType->setText("Threshold Type:");

        QComboBox* comboboxThreshType = new QComboBox();
        comboboxThreshType->setObjectName("comboboxThreshType");

        comboboxThreshType->addItem(Base::QEnumToQString(Base::e_OpenCVThresholdType::THRESH_BINARY));
        comboboxThreshType->addItem(Base::QEnumToQString(Base::e_OpenCVThresholdType::THRESH_BINARY_INV));
        comboboxThreshType->addItem(Base::QEnumToQString(Base::e_OpenCVThresholdType::THRESH_TRUNC));
        comboboxThreshType->addItem(Base::QEnumToQString(Base::e_OpenCVThresholdType::THRESH_TOZERO));
        comboboxThreshType->addItem(Base::QEnumToQString(Base::e_OpenCVThresholdType::THRESH_TOZERO_INV));

        layout->addWidget(labelThresh);
        layout->addWidget(spinboxTresholdValue);
        layout->addWidget(labelThreshMax);
        layout->addWidget(spinboxTresholdValueMax);
        layout->addWidget(labelThreshType);
        layout->addWidget(comboboxThreshType);
    }
    else if(function == "AdaptiveThreshold")
    {
        //Argument 1
        QLabel* labelThreshMax = new QLabel();
        labelThreshMax->setText("Threshold Max Value:");

        QDoubleSpinBox* spinboxTresholdValueMax = new QDoubleSpinBox();
        spinboxTresholdValueMax->setObjectName("spinboxTresholdValueMax");
        spinboxTresholdValueMax->setRange(0.0, 255);
        spinboxTresholdValueMax->setDecimals(1);
        spinboxTresholdValueMax->setValue(210.0);

        //Argument 2
        QLabel* labelAdaptiveMethod = new QLabel();
        labelAdaptiveMethod->setText("Adaptive Method:");

        QComboBox* comboboxAdaptiveMethod = new QComboBox();
        comboboxAdaptiveMethod->setObjectName("comboboxAdaptiveMethod");
        comboboxAdaptiveMethod->addItem(Base::QEnumToQString(Base::e_OpenCVAdaptivThresholdMethod::THRESH_MEAN_C));
        comboboxAdaptiveMethod->addItem(Base::QEnumToQString(Base::e_OpenCVAdaptivThresholdMethod::THRESH_GAUSSIAN_C));

        //Argument 3
        QLabel* labelThreshType = new QLabel();
        labelThreshType->setText("Threshold Type:");

        QComboBox* comboboxThreshType = new QComboBox();
        comboboxThreshType->setObjectName("comboboxThreshType");
        comboboxThreshType->addItem(Base::QEnumToQString(Base::e_OpenCVAdaptiveThresholdType::THRESH_ADAPTIV_BINARY));
        comboboxThreshType->addItem(Base::QEnumToQString(Base::e_OpenCVAdaptiveThresholdType::THRESH_ADAPTIV_BINARY_INV));

        //Argument 4
        QLabel* labelBlockSize = new QLabel();
        labelBlockSize->setText("Block Size:");

        QComboBox* comboboxBlockSize = new QComboBox();
        comboboxBlockSize->setObjectName("comboboxBlockSize");
        comboboxBlockSize->addItem(QString::number(static_cast<int>(Base::e_OpenCVBlockSize::BLOCKSIZE_3)));
        comboboxBlockSize->addItem(QString::number(static_cast<int>(Base::e_OpenCVBlockSize::BLOCKSIZE_5)));
        comboboxBlockSize->addItem(QString::number(static_cast<int>(Base::e_OpenCVBlockSize::BLOCKSIZE_7)));
        comboboxBlockSize->addItem(QString::number(static_cast<int>(Base::e_OpenCVBlockSize::BLOCKSIZE_9)));
        comboboxBlockSize->addItem(QString::number(static_cast<int>(Base::e_OpenCVBlockSize::BLOCKSIZE_11)));

        //Argument 5
        QLabel* labelConstantC = new QLabel();
        labelConstantC->setText("Constant C:");

        QDoubleSpinBox* spinBoxConstantC = new QDoubleSpinBox();
        spinBoxConstantC->setObjectName("spinboxConstantC");
        spinBoxConstantC->setRange(-1000.0, 1000);
        spinBoxConstantC->setValue(0);

        layout->addWidget(labelThreshMax);
        layout->addWidget(spinboxTresholdValueMax);
        layout->addWidget(labelAdaptiveMethod);
        layout->addWidget(comboboxAdaptiveMethod);
        layout->addWidget(labelThreshType);
        layout->addWidget(comboboxThreshType);
        layout->addWidget(labelBlockSize);
        layout->addWidget(comboboxBlockSize);
        layout->addWidget(labelConstantC);
        layout->addWidget(spinBoxConstantC);
    }
    else if(function == "MedianBlur")
    {
        //Argument 1
        QLabel* labelKSize = new QLabel();
        labelKSize->setText("KSize:");

        QComboBox* comboboxKSize = new QComboBox();
        comboboxKSize->setObjectName("comboboxKSize");
        comboboxKSize->addItem(QString::number(static_cast<int>(Base::e_OpenCVKSize::KSIZE_3)));
        comboboxKSize->addItem(QString::number(static_cast<int>(Base::e_OpenCVKSize::KSIZE_5)));
        comboboxKSize->addItem(QString::number(static_cast<int>(Base::e_OpenCVKSize::KSIZE_7)));
        comboboxKSize->addItem(QString::number(static_cast<int>(Base::e_OpenCVKSize::KSIZE_9)));
        comboboxKSize->addItem(QString::number(static_cast<int>(Base::e_OpenCVKSize::KSIZE_11)));

        layout->addWidget(labelKSize);
        layout->addWidget(comboboxKSize);
    }
    else if(function == "Blur")
    {
        //Argument 1a
        QLabel* labelKSizeWidth = new QLabel();
        labelKSizeWidth->setText("KSize Width:");

        QSpinBox* spinBoxKSizeWidth = new QSpinBox();
        spinBoxKSizeWidth->setObjectName("spinBoxKSizeWidth");
        spinBoxKSizeWidth->setRange(0, 2048);
        spinBoxKSizeWidth->setValue(3);

        //Argument 1b
        QLabel* labelKSizeHeight = new QLabel();
        labelKSizeHeight->setText("KSize Height:");

        QSpinBox* spinBoxKSizeHeight = new QSpinBox();
        spinBoxKSizeHeight->setObjectName("spinBoxKSizeHeight");
        spinBoxKSizeHeight->setRange(0, 2048);
        spinBoxKSizeHeight->setValue(3);

        //Argument 2
        QLabel* labelBorderType = new QLabel();
        labelBorderType->setText("BorderType:");

        QComboBox* comboboxBorderType = new QComboBox();
        comboboxBorderType->setObjectName("comboboxBorderType");
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_CONSTANT));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_REPLICATE));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_REFLECT));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_WRAP));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_REFLECT_101));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_TRANSPARENT));
        comboboxBorderType->setCurrentIndex(4); //BORDER_REFLECT_101 is the default value

        layout->addWidget(labelKSizeWidth);
        layout->addWidget(spinBoxKSizeWidth);
        layout->addWidget(labelKSizeHeight);
        layout->addWidget(spinBoxKSizeHeight);
        layout->addWidget(labelBorderType);
        layout->addWidget(comboboxBorderType);
    }
    else if(function == "GaussianBlur")
    {
        //Argument 1a
        QLabel* labelKSizeWidth = new QLabel();
        labelKSizeWidth->setText("KSize Width:");

        QSpinBox* spinBoxKSizeWidth = new QSpinBox();
        spinBoxKSizeWidth->setObjectName("spinBoxKSizeWidth");
        spinBoxKSizeWidth->setRange(0, 2048);
        spinBoxKSizeWidth->setValue(3);

        //Argument 1b
        QLabel* labelKSizeHeight = new QLabel();
        labelKSizeHeight->setText("KSize Height:");

        QSpinBox* spinBoxKSizeHeight = new QSpinBox();
        spinBoxKSizeHeight->setObjectName("spinBoxKSizeHeight");
        spinBoxKSizeHeight->setRange(0, 2048);
        spinBoxKSizeHeight->setValue(3);

        //Argument 2
        QLabel* labelSigmaX = new QLabel();
        labelSigmaX->setText("SigmaX:");

        QDoubleSpinBox* spinBoxSigmaX = new QDoubleSpinBox();
        spinBoxSigmaX->setObjectName("spinBoxSigmaX");
        spinBoxSigmaX->setRange(1, 10);
        spinBoxSigmaX->setValue(0);

        //Argument 3
        QLabel* labelSigmaY = new QLabel();
        labelSigmaY->setText("SigmaY:");

        QDoubleSpinBox* spinBoxSigmaY = new QDoubleSpinBox();
        spinBoxSigmaY->setObjectName("spinBoxSigmaY");
        spinBoxSigmaY->setRange(0, 10);
        spinBoxSigmaY->setValue(0);

        //Argument 4
        QLabel* labelBorderType = new QLabel();
        labelBorderType->setText("BorderType:");

        QComboBox* comboboxBorderType = new QComboBox();
        comboboxBorderType->setObjectName("comboboxBorderType");
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_CONSTANT));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_REPLICATE));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_REFLECT));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_WRAP));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_REFLECT_101));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_TRANSPARENT));
        comboboxBorderType->setCurrentIndex(4); //BORDER_REFLECT_101 is the default value

        layout->addWidget(labelKSizeWidth);
        layout->addWidget(spinBoxKSizeWidth);
        layout->addWidget(labelKSizeHeight);
        layout->addWidget(spinBoxKSizeHeight);
        layout->addWidget(labelSigmaX);
        layout->addWidget(spinBoxSigmaX);
        layout->addWidget(labelSigmaY);
        layout->addWidget(spinBoxSigmaY);
        layout->addWidget(labelBorderType);
        layout->addWidget(comboboxBorderType);
    }
    else if(function == "BitwiseNot")
    {
        //No Arguments
    }
    else if(function == "Pow")
    {
        //Argument 1
        QLabel* labelPow = new QLabel();
        labelPow->setText("Power:");

        QDoubleSpinBox* spinBoxPower = new QDoubleSpinBox();
        spinBoxPower->setObjectName("spinBoxPower");
        spinBoxPower->setRange(0,1);
        spinBoxPower->setValue(0.5);

        layout->addWidget(labelPow);
        layout->addWidget(spinBoxPower);
    }
    else if(function == "Erode")
    {
        //Argument 1
        QLabel* labelIterations = new QLabel();
        labelIterations->setText("Iterations:");

        QSpinBox* spinBoxIterations = new QSpinBox();
        spinBoxIterations->setObjectName("spinBoxIterations");
        spinBoxIterations->setRange(1, 10);
        spinBoxIterations->setValue(1);

        //Argument 2
        QLabel* labelBorderType = new QLabel();
        labelBorderType->setText("Border Type:");

        QComboBox* comboboxBorderType = new QComboBox();
        comboboxBorderType->setObjectName("comboboxBorderType");
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_CONSTANT));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_REPLICATE));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_REFLECT));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_WRAP));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_REFLECT_101));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_TRANSPARENT));
        comboboxBorderType->setCurrentIndex(4); //BORDER_REFLECT_101 is the default value

        //Argument 2
        QLabel* labelMorphShapes = new QLabel();
        labelMorphShapes->setText("Morph Shapes:");

        QComboBox* comboboxMorphShapes = new QComboBox();
        comboboxMorphShapes->setObjectName("comboboxMorphShapes");
        comboboxMorphShapes->addItem(Base::QEnumToQString(Base::e_OPenCVMorphShapes::MORPH_RECT));
        comboboxMorphShapes->addItem(Base::QEnumToQString(Base::e_OPenCVMorphShapes::MORPH_CROSS));
        comboboxMorphShapes->addItem(Base::QEnumToQString(Base::e_OPenCVMorphShapes::MORPH_ELLIPSE));

        //Argument 2
        QLabel* labelErosionSize = new QLabel();
        labelErosionSize->setText("Erosion Size:");

        QComboBox* comboboxErosionSize = new QComboBox();
        comboboxErosionSize->setObjectName("comboboxErosionSize");
        comboboxErosionSize->addItem(Base::QEnumToQString(Base::e_OpenCVErosionSize::ErosionSize_1));
        comboboxErosionSize->addItem(Base::QEnumToQString(Base::e_OpenCVErosionSize::ErosionSize_2));
        comboboxErosionSize->addItem(Base::QEnumToQString(Base::e_OpenCVErosionSize::ErosionSize_3));
        comboboxErosionSize->addItem(Base::QEnumToQString(Base::e_OpenCVErosionSize::ErosionSize_4));
        comboboxErosionSize->addItem(Base::QEnumToQString(Base::e_OpenCVErosionSize::ErosionSize_5));

        layout->addWidget(labelIterations);
        layout->addWidget(spinBoxIterations);
        layout->addWidget(labelBorderType);
        layout->addWidget(comboboxBorderType);
        layout->addWidget(labelMorphShapes);
        layout->addWidget(comboboxMorphShapes);
        layout->addWidget(labelErosionSize);
        layout->addWidget(comboboxErosionSize);
    }
    else if(function == "Sobel")
    {
        //Argument 1
        QLabel* labelDx = new QLabel();
        labelDx->setText("dx:");

        QSpinBox* spinBoxDx = new QSpinBox();
        spinBoxDx->setObjectName("spinBoxDx");
        spinBoxDx->setRange(0, 2048);
        spinBoxDx->setValue(1);

        //Argument 2
        QLabel* labelDy = new QLabel();
        labelDy->setText("dy:");

        QSpinBox* spinBoxDy = new QSpinBox();
        spinBoxDy->setObjectName("spinBoxDy");
        spinBoxDy->setRange(0, 2048);
        spinBoxDy->setValue(0);

        //Argument 3
        QLabel* labelKSize = new QLabel();
        labelKSize->setText("KSize:");

        QComboBox* comboboxKSize = new QComboBox();
        comboboxKSize->setObjectName("comboboxKSize");
        comboboxKSize->addItem(Base::QEnumToQString(Base::e_OpenCVKSizeExt::KSIZE_1));
        comboboxKSize->addItem(Base::QEnumToQString(Base::e_OpenCVKSizeExt::KSIZE_3));
        comboboxKSize->addItem(Base::QEnumToQString(Base::e_OpenCVKSizeExt::KSIZE_5));
        comboboxKSize->addItem(Base::QEnumToQString(Base::e_OpenCVKSizeExt::KSIZE_7));

        //Argument 4
        QLabel* labelScale = new QLabel();
        labelScale->setText("Scale:");

        QDoubleSpinBox* spinBoxScale = new QDoubleSpinBox();
        spinBoxScale->setObjectName("spinBoxScale");
        spinBoxScale->setRange(1,10);
        spinBoxScale->setValue(1);

        //Argument 5
        QLabel* labelBorderType = new QLabel();
        labelBorderType->setText("BorderType:");

        QComboBox* comboboxBorderType = new QComboBox();
        comboboxBorderType->setObjectName("comboboxBorderType");
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_CONSTANT));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_REPLICATE));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_REFLECT));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_WRAP));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_REFLECT_101));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_TRANSPARENT));
        comboboxBorderType->setCurrentIndex(4); //BORDER_REFLECT_101 is the default value

        layout->addWidget(labelDx);
        layout->addWidget(spinBoxDx);
        layout->addWidget(labelDy);
        layout->addWidget(spinBoxDy);
        layout->addWidget(labelKSize);
        layout->addWidget(comboboxKSize);
        layout->addWidget(labelScale);
        layout->addWidget(spinBoxScale);
        layout->addWidget(labelBorderType);
        layout->addWidget(comboboxBorderType);
    }
    else if(function == "Laplacian")
    {
        //Argument 1
        QLabel* labelKSize = new QLabel();
        labelKSize->setText("KSize:");

        QComboBox* comboboxKSize = new QComboBox();
        comboboxKSize->setObjectName("comboboxKSize");
        comboboxKSize->addItem(Base::QEnumToQString(Base::e_OpenCVKSizeExt::KSIZE_1));
        comboboxKSize->addItem(Base::QEnumToQString(Base::e_OpenCVKSizeExt::KSIZE_3));
        comboboxKSize->addItem(Base::QEnumToQString(Base::e_OpenCVKSizeExt::KSIZE_5));
        comboboxKSize->addItem(Base::QEnumToQString(Base::e_OpenCVKSizeExt::KSIZE_7));

        //Argument 2
        QLabel* labelScale = new QLabel();
        labelScale->setText("Scale:");

        QDoubleSpinBox* spinBoxScale = new QDoubleSpinBox();
        spinBoxScale->setObjectName("spinBoxScale");
        spinBoxScale->setRange(1, 10);
        spinBoxScale->setValue(1);

        //Argument 3
        QLabel* labelDelta = new QLabel();
        labelDelta->setText("Delta:");

        QDoubleSpinBox* spinBoxDelta = new QDoubleSpinBox();
        spinBoxDelta->setObjectName("spinBoxDelta");
        spinBoxDelta->setRange(0, 2048);
        spinBoxDelta->setValue(0);

        //Argument 4
        QLabel* labelBorderType = new QLabel();
        labelBorderType->setText("BorderType:");

        QComboBox* comboboxBorderType = new QComboBox();
        comboboxBorderType->setObjectName("comboboxBorderType");
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_CONSTANT));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_REPLICATE));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_REFLECT));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_WRAP));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_REFLECT_101));
        comboboxBorderType->addItem(Base::QEnumToQString(Base::e_OPenCVBorderType::BORDER_TRANSPARENT));
        comboboxBorderType->setCurrentIndex(4); //BORDER_REFLECT_101 is the default value

        layout->addWidget(labelKSize);
        layout->addWidget(comboboxKSize);
        layout->addWidget(labelScale);
        layout->addWidget(spinBoxScale);
        layout->addWidget(labelDelta);
        layout->addWidget(spinBoxDelta);
        layout->addWidget(labelBorderType);
        layout->addWidget(comboboxBorderType);
    }
    else if(function == "AddWeighted")
    {
        //Argument 1
        QLabel* labelAlpha = new QLabel();
        labelAlpha->setText("Alpha:");

        QDoubleSpinBox* spinBoxAlpha = new QDoubleSpinBox();
        spinBoxAlpha->setObjectName("spinBoxAlpha");
        spinBoxAlpha->setRange(0,1);
        spinBoxAlpha->setValue(0.5);

        //Argument 2
        QLabel* labelBeta = new QLabel();
        labelBeta->setText("Beta:");

        QDoubleSpinBox* spinBoxBeta = new QDoubleSpinBox();
        spinBoxBeta->setObjectName("spinBoxBeta");
        spinBoxBeta->setRange(0,1);
        spinBoxBeta->setValue(0.5);

        //Argument 3
        QLabel* labelGamma = new QLabel();
        labelGamma->setText("Gamma:");

        QDoubleSpinBox* spinBoxGamma = new QDoubleSpinBox();
        spinBoxGamma->setObjectName("spinBoxGamma");
        spinBoxGamma->setRange(0,2048);
        spinBoxGamma->setValue(0.0);

        layout->addWidget(labelAlpha);
        layout->addWidget(spinBoxAlpha);
        layout->addWidget(labelBeta);
        layout->addWidget(spinBoxBeta);
        layout->addWidget(labelGamma);
        layout->addWidget(spinBoxGamma);
    }
    else if(function == "CascadeClassifier")
    {
        //Argument 1
        QLabel* labelCascades = new QLabel();
        labelCascades->setText("Cascade:");

        QComboBox* comboboxCascade = new QComboBox();
        comboboxCascade->setObjectName("comboboxCascade");

        QDir directory("data/haarcascades");
        if(directory.isReadable() && !directory.isEmpty())
        {
            QStringList haarcascades = directory.entryList(QStringList("*.xml"), QDir::Files);
            comboboxCascade->addItems(haarcascades);
        }

        layout->addWidget(labelCascades);
        layout->addWidget(comboboxCascade);
    }
}
