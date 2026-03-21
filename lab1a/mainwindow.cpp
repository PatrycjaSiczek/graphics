#include "mainwindow.h"
#include "image_algorithm.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton,
            SIGNAL(clicked(bool)),
            this,
            SLOT(button_pushed()));
    connect(ui->horizontalSlider,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(slider_changed_brightness(int)));
    connect(ui->horizontalSlider_2,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(slider_changed_contrast(int)));
    connect(ui->horizontalSlider_3,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(slider_changed_gamma(int)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::button_pushed(){
    QString fname = QFileDialog::getOpenFileName();
    if( !fname.isEmpty() ){
        qDebug() << fname;
        bool ok = org_image.load(fname);
        if(ok){
            image = org_image;
            qDebug() << image;
            ui->label->setPixmap(QPixmap::fromImage(image));
        } else {
            qDebug() << "Blad ladowania obrazu";
        }
    }
}

void MainWindow::slider_changed_brightness(int v){
    if(!org_image.isNull()){
        double v_g = ui->horizontalSlider_3->value() / 25.0;
        int v_c = ui->horizontalSlider_2->value();
        change_brightness(org_image, &image, v);
        change_contrast(image, &image, v_c);
        changed_gamma(image, &image, 1.0 / v_g);
        ui->label->setPixmap(
            QPixmap::fromImage(image));
        update_histogram(image);
    }
}

void MainWindow::slider_changed_contrast(int v){
    if(!org_image.isNull()){
        int v_b = ui->horizontalSlider->value();
        double v_g = ui->horizontalSlider_3->value() / 25.0;
        change_brightness(org_image, &image, v_b);
        change_contrast(image, &image, v);
        changed_gamma(image, &image, 1.0 / v_g);
        ui->label->setPixmap(
            QPixmap::fromImage(image));
        update_histogram(image);
    }
}

void MainWindow::slider_changed_gamma(int v){
    if(!org_image.isNull()){
        double v_g = v / 25.0;
        int v_b = ui->horizontalSlider->value();
        int v_c = ui->horizontalSlider_2->value();
        change_brightness(org_image, &image, v_b);
        change_contrast(image, &image, v_c);
        changed_gamma(image, &image, 1.0 / v_g);
        ui->label->setPixmap(
            QPixmap::fromImage(image));
        update_histogram(image);
    }
}
void MainWindow::update_histogram(const QImage &src) {
    if(src.isNull()) return;

    int histR[256], histG[256], histB[256];
    histograms(src, histR, histG, histB);

    auto draw_single = [&](int* data, QColor color) {
        QImage img(256, 256, QImage::Format_RGB32);
        img.fill(Qt::black);

        int max = 0;
        for(int i=0; i<256; i++)
            if(data[i] > max)
                max = data[i];

        if(max > 0) {
            for(int x=0; x<256; x++) {
                int h = (data[x] * 255) / max;
                for(int y=0; y<h; y++) {
                    img.setPixel(x, 255-y, color.rgb());
                }
            }
        }
        return QPixmap::fromImage(img);
    };

    ui->label_5->setPixmap(draw_single(histR, Qt::red));
    ui->label_6->setPixmap(draw_single(histG, Qt::green));
    ui->label_7->setPixmap(draw_single(histB, Qt::blue));
}



