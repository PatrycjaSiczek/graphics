#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QDebug>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    isFirstClick = true;
    currentMode = MODE_LINE;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_L) {
        currentMode = MODE_LINE;
        isFirstClick = true;
    }
    else if (e->key() == Qt::Key_C) {
        currentMode = MODE_CIRCLE;
        isFirstClick = true;
    }
    else if (e->key() == Qt::Key_R) {
        image.fill(Qt::white);
        isFirstClick = true;
        update();
    }
}

void MainWindow::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        int x = e->pos().x();
        int y = e->pos().y();

        if (isFirstClick) {
            startX = x;
            startY = y;
            isFirstClick = false;
            plotPixel(startX, startY, qRgb(255, 0, 0));

        } else {
            if (currentMode == MODE_LINE) {
                drawLine(startX, startY, x, y, qRgb(0, 255, 255));
            } else if (currentMode == MODE_CIRCLE) {
                int r = std::round(std::hypot(x - startX, y - startY));
                drawCircle(startX, startY, r, qRgb(0, 255, 0));
            }
            isFirstClick = true;
        }
        update();
    }
}

void MainWindow::resizeEvent(QResizeEvent* e){
    image = QImage(e->size().width(), e->size().height(), QImage::Format_RGB32);
    image.fill(Qt::white);
}

void MainWindow::paintEvent(QPaintEvent* e)
{
    QPainter paint(this);
    paint.drawImage(0, 0, image);
}


void MainWindow::plotPixel(int x, int y, QRgb color) {
    if (x >= 0 && x < image.width() && y >= 0 && y < image.height()) {
        image.setPixel(x, y, color);
    }
}


void MainWindow::drawLine(int x0, int y0, int x1, int y1, QRgb color) {
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        plotPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void MainWindow::drawCircle(int xc, int yc, int r, QRgb color) {
    int x = 0;
    int y = r;
    int d = 1 - r;

    while (x <= y) {
        plotPixel(xc + x, yc + y, color);
        plotPixel(xc - x, yc + y, color);
        plotPixel(xc + x, yc - y, color);
        plotPixel(xc - x, yc - y, color);
        plotPixel(xc + y, yc + x, color);
        plotPixel(xc - y, yc + x, color);
        plotPixel(xc + y, yc - x, color);
        plotPixel(xc - y, yc - x, color);

        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}
