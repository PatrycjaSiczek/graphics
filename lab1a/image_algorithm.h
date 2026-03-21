#ifndef IMAGE_ALGORITHM_H
#define IMAGE_ALGORITHM_H
#include <QImage>

void change_brightness(const QImage& src, QImage* dst, int dv);
void change_contrast(const QImage& src, QImage* dst, int dv);
void changed_gamma(const QImage& src, QImage* dst, double dv);
void histograms(const QImage& src, int* histR, int* histG, int* histB);
#endif // IMAGE_ALGORITHM_H
