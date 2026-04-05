#include "image_algorithm.h"
#include <cmath>


QRgb get_bilinear_pixel(const QImage& img, double x, double y) {
    int x1 = std::floor(x);
    int y1 = std::floor(y);
    int x2 = x1 + 1;
    int y2 = y1 + 1;

    if (x1 >= 0 && x2 < img.width() && y1 >= 0 && y2 < img.height()) {
        double a = x - x1;
        double b = y - y1;

        QRgb p11 = img.pixel(x1, y1);
        QRgb p21 = img.pixel(x2, y1);
        QRgb p12 = img.pixel(x1, y2);
        QRgb p22 = img.pixel(x2, y2);

        int r = (1-a)*(1-b)*qRed(p11) + a*(1-b)*qRed(p21) + (1-a)*b*qRed(p12) + a*b*qRed(p22);
        int g = (1-a)*(1-b)*qGreen(p11) + a*(1-b)*qGreen(p21) + (1-a)*b*qGreen(p12) + a*b*qGreen(p22);
        int b_val = (1-a)*(1-b)*qBlue(p11) + a*(1-b)*qBlue(p21) + (1-a)*b*qBlue(p12) + a*b*qBlue(p22);

        return qRgb(r, g, b_val);
    }
    return qRgb(0,0,0);
}

void rotate(const QImage& org, QImage* dst, double alfa, int interp_mode) {
    if (org.isNull()) return;
    int W = org.width();
    int H = org.height();

    *dst = QImage(W, H, org.format());
    dst->fill(Qt::black);

    double cx = W / 2.0;
    double cy = H / 2.0;
    double rad = -alfa * M_PI / 180.0;
    double cos_a = std::cos(rad);
    double sin_a = std::sin(rad);

    for (int y = 0; y < H; ++y) {
        double dy = y - cy;
        for (int x = 0; x < W; ++x) {
            double dx = x - cx;

            double rx = dx * cos_a - dy * sin_a;
            double ry = dx * sin_a + dy * cos_a;

            double src_x = rx + cx;
            double src_y = ry + cy;

            if (interp_mode == 0) {
                int ix = std::round(src_x);
                int iy = std::round(src_y);
                if (ix >= 0 && ix < W && iy >= 0 && iy < H) {
                    dst->setPixel(x, y, org.pixel(ix, iy));
                }
            } else {
                if (src_x >= 0 && src_x < W - 1 && src_y >= 0 && src_y < H - 1) {
                    dst->setPixel(x, y, get_bilinear_pixel(org, src_x, src_y));
                }
            }
        }
    }
}

void scale(const QImage& org, QImage* dst, double sx, double sy, int interp_mode) {
    if (org.isNull()) return;
    if (std::abs(sx) < 0.001) sx = 0.001;
    if (std::abs(sy) < 0.001) sy = 0.001;

    int W = std::round(org.width() * sx);
    int H = std::round(org.height() * sy);

    *dst = QImage(W, H, org.format());
    dst->fill(Qt::black);

    for (int y = 0; y < H; ++y) {
        double src_y = y / sy;
        for (int x = 0; x < W; ++x) {
            double src_x = x / sx;

            if (interp_mode == 0) {
                int ix = std::round(src_x);
                int iy = std::round(src_y);
                if (ix >= 0 && ix < org.width() && iy >= 0 && iy < org.height()) {
                    dst->setPixel(x, y, org.pixel(ix, iy));
                }
            } else {
                if (src_x >= 0 && src_x < org.width() - 1 && src_y >= 0 && src_y < org.height() - 1) {
                    dst->setPixel(x, y, get_bilinear_pixel(org, src_x, src_y));
                }
            }
        }
    }
}
