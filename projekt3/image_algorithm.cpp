#include "image_algorithm.h"
#include <algorithm>

QRgb conv(const uchar* image, int x, int y, int width, int height, int bytesPerLine, int* mask, int r) {
    int r_acc = 0, g_acc = 0, b_acc = 0;
    int sum = 0;
    int size = 2 * r + 1;

    for (int my = -r; my <= r; ++my) {
        for (int mx = -r; mx <= r; ++mx) {
            int px = std::clamp(x + mx, 0, width - 1);
            int py = std::clamp(y + my, 0, height - 1);
            const uchar* pixel = image + (py * bytesPerLine) + (px * 4);
            int mask_index = (my + r) * size + (mx + r);
            int w = mask[mask_index];

            b_acc += pixel[0] * w;
            g_acc += pixel[1] * w;
            r_acc += pixel[2] * w;
            sum += w;
        }
    }
    if (sum == 0)
        sum = 1;

    int final_r = std::clamp(r_acc / sum, 0, 255);
    int final_g = std::clamp(g_acc / sum, 0, 255);
    int final_b = std::clamp(b_acc / sum, 0, 255);

    return qRgb(final_r, final_g, final_b);
}


void process_conv(const QImage& src, QImage* dst, int* mask, int r) {

    QImage src32 = src.convertToFormat(QImage::Format_RGB32);
    *dst = src32.copy();
    const uchar* img_data = src32.constBits();
    int w = src32.width();
    int h = src32.height();
    int bpl = src32.bytesPerLine();

    for (int y = 0; y < h; ++y) {
        QRgb* dst_line = (QRgb*)dst->scanLine(y);
        for (int x = 0; x < w; ++x) {
            dst_line[x] = conv(img_data, x, y, w, h, bpl, mask, r);
        }
    }
}
