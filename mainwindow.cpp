#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_checkBox_Vertices_stateChanged(int arg)
{
    // When the checkbox is checked, this function is appealed with arg = 2
    // When it is uncheked, this function is appealed with arg = 0
    // The boolean show_vertecis is updated considering arg.
    ui->rendering->show_vertecis = arg == 2;
}

void MainWindow::on_checkBox_Edges_stateChanged(int arg)
{
    // When the checkbox is checked, this function is appealed with arg = 2
    // When it is uncheked, this function is appealed with arg = 0
    // The boolean show_edges is updated considering arg.
    ui->rendering->show_edges = arg == 2;
}

void MainWindow::on_checkBox_Faces_stateChanged(int arg)
{
    // When the checkbox is checked, this function is appealed with arg = 2
    // When it is uncheked, this function is appealed with arg = 0
    // The boolean show_faces is updated considering arg.
    ui->rendering->show_faces = arg == 2;
}

void MainWindow::on_checkBox_Laplacian_stateChanged(int arg)
{
    // When the checkbox is checked, this function is appealed with arg = 2
    // When it is uncheked, this function is appealed with arg = 0
    // The boolean show_laplacian is updated considering arg.
    ui->rendering->show_laplacian = arg == 2;
}

void MainWindow::on_Laplacian_size_Slider_sliderMoved(int position)
{
    double exponent = (double(position) - 60) / 10;
    ui->rendering->coef_laplacian = pow(10, exponent);
}
