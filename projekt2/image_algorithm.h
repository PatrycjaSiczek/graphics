#ifndef IMAGE_ALGORITHM_H
#define IMAGE_ALGORITHM_H
#include <QImage>
#include <cmath>
#include <vector>

void change_brightness(const QImage& src, QImage* dst, int dv);
void change_contrast(const QImage& src, QImage* dst, int dv);
void changed_gamma(const QImage& src, QImage* dst, double dv);

void rgbZxyz(double r, double g, double b, double &x, double &y, double &z);
void xyzZlab(double x, double y, double z, double &L, double &a, double &b);
void labZxyz(double L, double a, double b, double &x, double &y, double &z);
void xyzZrgb(double x, double y, double z, double &r, double &g, double &b);

void rgbZhsl(double r, double g, double b, double &h, double &s, double &l);
void hslZrgb(double h, double s, double l, double &r, double &g, double &b);

void hsl(const QImage &src, QImage *dst, double dH, double dS, double dL);
void lab(const QImage &src, QImage *dst, double dL, double da, double db);

void apply_convolution(const QImage& src, QImage* dst, const std::vector<std::vector<double>>& mask);
void histograms(const QImage& src, int* r, int* g, int* b);

#endif // IMAGE_ALGORITHM_H
