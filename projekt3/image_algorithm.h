#ifndef IMAGE_ALGORITHM_H
#define IMAGE_ALGORITHM_H

#include <QImage>

QRgb conv(const uchar* image, int x, int y, int width, int height, int bytesPerLine, int* mask, int r);
void process_conv(const QImage& src, QImage* dst, int* mask, int r);

#endif // IMAGE_ALGORITHM_H
