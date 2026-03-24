#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "image_algorithm.h"
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
            SLOT(update_view()));
    connect(ui->horizontalSlider_2,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(update_view()));
    connect(ui->horizontalSlider_3,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(update_view()));
    connect(ui->comboBox, &QComboBox::currentIndexChanged,
            this,
            &MainWindow::mode_changed);
    connect(ui->comboBox,
            SIGNAL(currentIndexChanged(int)),
            this,
            SLOT(update_view()));
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

void MainWindow::update_view() {

    if(org_image.isNull()) return;
    QImage temp_img = org_image.copy();
    int index = ui->comboBox->currentIndex();

    if (index == 0) {
        double h = ui->horizontalSlider->value();
        double s = ui->horizontalSlider_2->value() / 255.0;
        double l = ui->horizontalSlider_3->value() / 255.0;
        hsl(temp_img, &temp_img, h, s, l);
    } else if (index == 1) {
        double L = (ui->horizontalSlider->value() / 255.0) * 100.0;
        double a = (ui->horizontalSlider_2->value() / 255.0) * 128.0;
        double b = (ui->horizontalSlider_3->value() / 255.0) * 128.0;
        lab(temp_img, &temp_img, L, a, b);
    } else if (index == 2) {
        int v_b = ui->horizontalSlider->value();
        int v_c = ui->horizontalSlider_2->value();
        if (v_c > 250) v_c = 250;
        if (v_c < -250) v_c = -250;
        double gamma_val = pow(2.0, -ui->horizontalSlider_3->value() / 100.0);
        change_brightness(temp_img, &temp_img, v_b);
        change_contrast(temp_img, &temp_img, v_c);
        changed_gamma(temp_img, &temp_img, gamma_val);
    } else if (index == 3) {
        std::vector<std::vector<double>> blur_mask = {
            {1, 1, 1},
            {1, 1, 1},
            {1, 1, 1}
        };
        // std::vector<std::vector<double>> sharpen_mask = {
        //     { 0, -1,  0},
        //     {-1,  5, -1},
        //     { 0, -1,  0}
        // };

        apply_convolution(temp_img, &temp_img, blur_mask);
    }
    image = temp_img;
    ui->label->setPixmap(QPixmap::fromImage(image));
    update_histogram(image);
}

void MainWindow::update_histogram(const QImage &src) {

    if(src.isNull()) return;
    int histR[256], histG[256], histB[256];
    histograms(src, histR, histG, histB);

    QImage img(256, 256, QImage::Format_RGB32);
    img.fill(Qt::black);

    int max = 0;
    for(int i=0; i<256; i++) {
        if(histR[i] > max) max = histR[i];
        if(histG[i] > max) max = histG[i];
        if(histB[i] > max) max = histB[i];
    }

    if(max > 0) {
        for(int x=0; x<256; x++) {
            int hR = (int)((double)histR[x] / max * 255.0);
            int hG = (int)((double)histG[x] / max * 255.0);
            int hB = (int)((double)histB[x] / max * 255.0);

            for(int y=0; y<256; y++) {
                int py = 255 - y;
                int r = (y < hR) ? 255 : 0;
                int g = (y < hG) ? 255 : 0;
                int b = (y < hB) ? 255 : 0;

                if(r || g || b) {
                    img.setPixel(x, py, qRgb(r, g, b));
                }
            }
        }
    }

    ui->label_10->setPixmap(QPixmap::fromImage(img));

}

void MainWindow::mode_changed(int index){

    ui->horizontalSlider->setValue(0);
    ui->horizontalSlider_2->setValue(0);
    ui->horizontalSlider_3->setValue(0);

    if (index == 0) {
        ui->label_2->setText("Hue");
        ui->label_3->setText("Saturation");
        ui->label_4->setText("Lightness");
    } else if (index == 1){
        ui->label_2->setText("L");
        ui->label_3->setText("A");
        ui->label_4->setText("B");
    } else if (index == 2){
        ui->label_2->setText("Brightness");
        ui->label_3->setText("Contrast");
        ui->label_4->setText("Gamma");
    } else {
        ui->label_2->setText("");
        ui->label_3->setText("");
        ui->label_4->setText("");
    }
    update_view();
}
