#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_checkBox_Vertices_stateChanged(int arg1)
{
    ui->widget->show_vertecis = arg1==2;
}

void MainWindow::on_checkBox_Edges_stateChanged(int arg1)
{
    ui->widget->show_edges = arg1==2;
}

void MainWindow::on_checkBox_Faces_stateChanged(int arg1)
{
    ui->widget->show_faces = arg1==2;
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    double exponent = (double(position)-60)/10;
    ui->widget->coef_laplacian = pow(10, exponent);
}

void MainWindow::on_checkBox_Faces_2_stateChanged(int arg1)
{
    ui->widget->show_laplacian = arg1==2;
}
