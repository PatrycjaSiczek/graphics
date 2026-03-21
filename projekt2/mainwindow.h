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
    QImage org_image;
    QImage image;
    int histogram[256];
    int histR[256];
    int histG[256];
    int histB[256];
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;


public slots:
    void button_pushed();
    void slider_changed_brightness(int);
    void slider_changed_contrast(int);
    void slider_changed_gamma(int);
    void update_view();
    void update_histogram(const QImage &src);
    void mode_changed(int index);

};
#endif // MAINWINDOW_H
