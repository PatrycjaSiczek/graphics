#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "image_algorithm.h"
#include <QFileDialog>
#include <QDebug>
#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton,
            SIGNAL(clicked()),
            this,
            SLOT(button_pushed()));
    connect(ui->spinBox_r,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(valueChanged(int)));
    connect(ui->btn_apply,
            SIGNAL(clicked()),
            this,
            SLOT(apply_clicked()));
    connect(ui->btn_gauss,
            SIGNAL(clicked()),
            this,
            SLOT(m_gauss()));
    connect(ui->btn_laplace,
            SIGNAL(clicked()),
            this,
            SLOT(m_laplace()));
    connect(ui->btn_zeros,
            SIGNAL(clicked()),
            this,
            SLOT(m_zeros()));

    valueChanged(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::button_pushed(){
    QString fname = QFileDialog::getOpenFileName();
    if( !fname.isEmpty() ){
        bool ok = org_image.load(fname);
        if(ok){
            org_image = org_image.convertToFormat(QImage::Format_RGB32);
            image = org_image;
            ui->label_dst->setPixmap(QPixmap::fromImage(image));
        } else {
            qDebug() << "Blad ladowania obrazu";
        }
    }
}

void MainWindow::valueChanged(int r)
{
    int size = 2 * r + 1;
    ui->tableWidget->setRowCount(size);
    ui->tableWidget->setColumnCount(size);

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            if(!ui->tableWidget->item(i, j)){
                ui->tableWidget->setItem(i, j, new QTableWidgetItem("1"));
            }
        }
    }
}

void MainWindow::apply_clicked()
{
    if(org_image.isNull())
        return;
    int r = ui->spinBox_r->value();
    int size = 2 * r + 1;

    std::vector<int> mask(size * size);
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            QTableWidgetItem *item = ui->tableWidget->item(i, j);
            int value = item ? item->text().toInt() : 1;
            mask[i * size + j] = value;
        }
    }

    process_conv(org_image, &dst_image, mask.data(), r);
    ui->label_dst->setPixmap(QPixmap::fromImage(dst_image));
}

void MainWindow::set_mask(int r, const std::vector<int>& values) {
    ui->spinBox_r->setValue(r);
    int size = 2 * r + 1;
    for(int i = 0; i < size * size; i++) {
        ui->tableWidget->item(i / size, i % size)->setText(QString::number(values[i]));
    }
}

void MainWindow::m_gauss() {
    std::vector<int> gauss = {
        1, 2, 1,
        2, 4, 2,
        1, 2, 1
    };
    set_mask(1, gauss);
}

void MainWindow::m_laplace() {
    std::vector<int> laplace = {
        0, -1,  0,
        -1,  4, -1,
        0, -1,  0
    };
    set_mask(1, laplace);
}

void MainWindow::m_zeros() {
    std::vector<int> zeros = {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0
    };
    set_mask(1, zeros);
}
