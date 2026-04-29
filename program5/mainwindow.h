#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_line_clicked();
    void on_btn_circle_clicked();
    void on_btn_clear_clicked();

private:
    Ui::MainWindow *ui;

    QImage canvas;


    void update_canvas();

    void draw_line_bresenham(int x0, int y0, int x1, int y1, QRgb color);
    void draw_circle_midpoint(int xc, int yc, int r, QRgb color);

    void plot_pixel(int x, int y, QRgb color);
};

#endif // MAINWINDOW_H
