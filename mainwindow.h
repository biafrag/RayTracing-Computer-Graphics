#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QLabel>
#include <QMainWindow>
#include "image.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
     QLabel* inputLabel;

private slots:
    void on_actionRayTracing_triggered();

private:
    Ui::MainWindow *ui;
    QImage outputImage;
    QImage image;
};

#endif // MAINWINDOW_H
