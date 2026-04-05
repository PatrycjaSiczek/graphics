#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "image_algorithm.h"
#include <QFileDialog>
#include <QDebug>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->setRowCount(3);
    ui->tableWidget->setColumnCount(3);

    connect(ui->pushButton,
            SIGNAL(clicked()),
            this,
            SLOT(button_pushed()));
    connect(ui->horizontalSlider,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(update_view()));
    connect(ui->spin_sx,
            SIGNAL(valueChanged(double)),
            this,
            SLOT(update_view()));
    connect(ui->spin_sy,
            SIGNAL(valueChanged(double)),
            this,
            SLOT(update_view()));
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
        bool ok = org_image.load(fname);
        if(ok){
            org_image = org_image.convertToFormat(QImage::Format_RGB32);

            ui->spin_sx->blockSignals(true);
            ui->spin_sy->blockSignals(true);
            ui->horizontalSlider->blockSignals(true);

            ui->spin_sx->setValue(1.0);
            ui->spin_sy->setValue(1.0);
            ui->horizontalSlider->setValue(0);

            ui->spin_sx->blockSignals(false);
            ui->spin_sy->blockSignals(false);
            ui->horizontalSlider->blockSignals(false);

            update_view();
        } else {
            qDebug() << "Blad ladowania obrazu";
        }
    }
}

void MainWindow::update_view() {
    if(org_image.isNull())
        return;

    double alfa = ui->horizontalSlider->value();
    double sx = ui->spin_sx->value();
    double sy = ui->spin_sy->value();
    int interp_mode = ui->comboBox->currentIndex();

    QImage temp_image;

    rotate(org_image, &temp_image, alfa, interp_mode);
    scale(temp_image, &dst_image, sx, sy, interp_mode);

    ui->label->setPixmap(QPixmap::fromImage(dst_image));
    double rad = alfa * M_PI / 180.0;
    double c = std::cos(rad);
    double s = std::sin(rad);

    ui->tableWidget->setItem(0, 0, new QTableWidgetItem(QString::number(c * sx, 'f', 2)));
    ui->tableWidget->setItem(0, 1, new QTableWidgetItem(QString::number(-s * sy, 'f', 2)));
    ui->tableWidget->setItem(0, 2, new QTableWidgetItem("0"));

    ui->tableWidget->setItem(1, 0, new QTableWidgetItem(QString::number(s * sx, 'f', 2)));
    ui->tableWidget->setItem(1, 1, new QTableWidgetItem(QString::number(c * sy, 'f', 2)));
    ui->tableWidget->setItem(1, 2, new QTableWidgetItem("0"));

    ui->tableWidget->setItem(2, 0, new QTableWidgetItem("0"));
    ui->tableWidget->setItem(2, 1, new QTableWidgetItem("0"));
    ui->tableWidget->setItem(2, 2, new QTableWidgetItem("1"));
}
