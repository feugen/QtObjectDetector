#include "layouthandler.h"
#include "base.h"
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>

LayoutHandler::LayoutHandler(QObject *parent) : QObject(parent)
{

}

void LayoutHandler::createLayout(QLayout *layout, QString function)
{
    if(layout == nullptr) return void();
    if(function == "cvtColor")
    {
        QLabel* labelFormat = new QLabel();
        labelFormat->setText("Color Format:");

        QComboBox* comboboxFormat = new QComboBox();
        comboboxFormat->setObjectName("comboboxFormat");
        comboboxFormat->addItem("Gray");

        layout->addWidget(labelFormat);
        layout->addWidget(comboboxFormat);
    }
    else if(function == "threshold")
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
    else if(function == "adaptiveThreshold")
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
        comboboxBlockSize->addItem(QString::number(static_cast<int>(Base::e_OpenCVAdaptiveThresholdBlocksize::BLOCKSIZE_3)));
        comboboxBlockSize->addItem(QString::number(static_cast<int>(Base::e_OpenCVAdaptiveThresholdBlocksize::BLOCKSIZE_5)));
        comboboxBlockSize->addItem(QString::number(static_cast<int>(Base::e_OpenCVAdaptiveThresholdBlocksize::BLOCKSIZE_7)));
        comboboxBlockSize->addItem(QString::number(static_cast<int>(Base::e_OpenCVAdaptiveThresholdBlocksize::BLOCKSIZE_9)));
        comboboxBlockSize->addItem(QString::number(static_cast<int>(Base::e_OpenCVAdaptiveThresholdBlocksize::BLOCKSIZE_11)));

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
}
