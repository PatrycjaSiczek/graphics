#ifndef IMAGE_ALGORITHM_H
#define IMAGE_ALGORITHM_H

#include <QImage>

void rotate(const QImage& org, QImage* dst, double alfa, int interp_mode);
void scale(const QImage& org, QImage* dst, double sx, double sy, int interp_mode);

#endif // IMAGE_ALGORITHM_H
