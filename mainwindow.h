#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_checkBox_Vertices_stateChanged(int arg);  // Called when the "show vertices" checkbox is checked
    void on_checkBox_Edges_stateChanged(int arg);     // Called when the "show edges" checkbox is checked
    void on_checkBox_Faces_stateChanged(int arg);     // Called when the "show faces" checkbox is checked
    void on_checkBox_Laplacian_stateChanged(int arg); // Called when the "show Laplacian" checkbox is checked

    void on_Laplacian_size_Slider_sliderMoved(int position); // Called the the slidebar of laplacian coef is slided

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
