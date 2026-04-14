#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QResizeEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void paintEvent(QPaintEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

private:
    Ui::MainWindow *ui;
    QImage image;

    bool isFirstClick;
    int startX;
    int startY;

    enum DrawMode { MODE_LINE, MODE_CIRCLE };
    DrawMode currentMode;

    void plotPixel(int x, int y, QRgb color);
    void drawLine(int x0, int y0, int x1, int y1, QRgb color);
    void drawCircle(int xc, int yc, int r, QRgb color);
};

#endif // MAINWINDOW_H
