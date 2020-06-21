#include "qtobjectdetector.h"
#include "ui_qtobjectdetector.h"
#include <opencv2/opencv.hpp>

#include <QImage>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

QtObjectDetector::QtObjectDetector(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QtObjectDetector)
{
    ui->setupUi(this);
    cv::Mat inputImage = cv::imread("/home/eugenf/Dokumente/rsz_1efischer.jpg");

    QImage * imgIn = new QImage(static_cast<uchar*>(inputImage.data), inputImage.cols, inputImage.rows, static_cast<int>(inputImage.step), QImage::Format_BGR888);
    QGraphicsPixmapItem * item = new QGraphicsPixmapItem(QPixmap::fromImage(*imgIn));
    QGraphicsScene* scene = new QGraphicsScene(this);

    scene->addItem(item);
    ui->graphicsView_PhotoLoader->setScene(scene);
    ui->graphicsView_PhotoLoader->show();
}

QtObjectDetector::~QtObjectDetector()
{
    delete ui;
}

