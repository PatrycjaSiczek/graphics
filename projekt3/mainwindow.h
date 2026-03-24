#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <vector>

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
    void button_pushed();
    void valueChanged(int r);
    void apply_clicked();

    void m_gauss();
    void m_laplace();
    void m_zeros();

private:
    Ui::MainWindow *ui;

    QImage org_image;
    QImage dst_image;
    QImage image;

    void set_mask(int r, const std::vector<int>& values);
};

#endif // MAINWINDOW_H
