#ifndef OBJECTDETECTOR_H
#define OBJECTDETECTOR_H

#include <QMainWindow>
#include <QFileDialog>
#include <photoloader.h>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class QtObjectDetector; }
QT_END_NAMESPACE

class QtObjectDetector : public QMainWindow
{
    Q_OBJECT

public:
    QtObjectDetector(QWidget *parent = nullptr);
    ~QtObjectDetector();

private slots:
    void on_selectFileButton_clicked();
    void on_loadFilePushButton_clicked();
    void on_fileSelected(const QString &file);

    void on_autoSizeCheckBox_toggled(bool checked);

private:
    PhotoLoader * m_photoLoader = nullptr;
    QFileDialog * m_fileDialog = nullptr;
    Ui::QtObjectDetector *ui;
};
#endif // OBJECTDETECTOR_H
