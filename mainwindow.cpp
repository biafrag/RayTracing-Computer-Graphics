#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "raytracing.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->DadosWidget->setFixedSize(100,100);
    inputLabel = findChild<QLabel*>("inputLabel");
    //ui->inputLabel->setSizeIncrement(100,100);
    //image.scaled(image.size(),(float)ui->DadosWidget->width()/ui->DadosWidget->height());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionRayTracing_triggered()
{
    ui->DadosWidget->ZBuffer=false;
    //ui->DadosWidget->Ray=true;
    QImage image(ui->DadosWidget->width(), ui->DadosWidget->height(), QImage::Format_RGB32);
    int i;

    image=ui->DadosWidget->getImageRay();

    if( !image.isNull())
    {
        inputLabel->setPixmap(QPixmap::fromImage(image));
    }

}
