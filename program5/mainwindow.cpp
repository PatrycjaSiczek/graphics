#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    canvas = QImage(600, 600, QImage::Format_RGB32);
    on_btn_clear_clicked();

    connect(ui->pushButton,
            SIGNAL(clicked()), this, SLOT(on_btn_line_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_btn_circle_clicked()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_btn_clear_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_canvas() {
    ui->label_canvas->setPixmap(QPixmap::fromImage(canvas));
}

void MainWindow::on_btn_clear_clicked() {
    canvas.fill(Qt::white);
    update_canvas();
}

void MainWindow::plot_pixel(int x, int y, QRgb color) {
    if (x >= 0 && x < canvas.width() && y >= 0 && y < canvas.height()) {
        canvas.setPixel(x, y, color);
    }
}

void MainWindow::draw_line_bresenham(int x0, int y0, int x1, int y1, QRgb color) {

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        plot_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;

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

void MainWindow::draw_circle_midpoint(int xc, int yc, int r, QRgb color) {
    int x = 0;
    int y = r;
    int d = 1 - r;

    auto plot8 = [&](int px, int py) {
        plot_pixel(xc + px, yc + py, color);
        plot_pixel(xc - px, yc + py, color);
        plot_pixel(xc + px, yc - py, color);
        plot_pixel(xc - px, yc - py, color);
        plot_pixel(xc + py, yc + px, color);
        plot_pixel(xc - py, yc + px, color);
        plot_pixel(xc + py, yc - px, color);
        plot_pixel(xc - py, yc - px, color);
    };

    while (x <= y) {
        plot8(x, y);

        if (d < 0) {

            d += 2 * x + 3;
        } else {

            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}


void MainWindow::on_btn_line_clicked() {
    int x0 = ui->spin_x0->value();
    int y0 = ui->spin_y0->value();
    int x1 = ui->spin_x1->value();
    int y1 = ui->spin_y1->value();

    draw_line_bresenham(x0, y0, x1, y1, qRgb(255, 0, 0));
    update_canvas();
}

void MainWindow::on_btn_circle_clicked() {
    int xc = ui->spin_xc->value();
    int yc = ui->spin_yc->value();
    int r = ui->spin_r->value();

    draw_circle_midpoint(xc, yc, r, qRgb(0, 0, 255));
    update_canvas();
}
