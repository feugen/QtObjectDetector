#ifndef OBJECTDETECTOR_H
#define OBJECTDETECTOR_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class QtObjectDetector; }
QT_END_NAMESPACE

class QtObjectDetector : public QMainWindow
{
    Q_OBJECT

public:
    QtObjectDetector(QWidget *parent = nullptr);
    ~QtObjectDetector();

private:
    Ui::QtObjectDetector *ui;
};
#endif // OBJECTDETECTOR_H
