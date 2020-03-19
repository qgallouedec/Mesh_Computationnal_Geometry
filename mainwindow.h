#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_checkBox_Vertices_stateChanged(int arg1);

    void on_checkBox_Edges_stateChanged(int arg1);

    void on_checkBox_Faces_stateChanged(int arg1);

    void on_horizontalSlider_sliderMoved(int position);

    void on_checkBox_Faces_2_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
