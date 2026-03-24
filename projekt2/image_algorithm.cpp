#include "image_algorithm.h"
#include <vector>
#include <algorithm>

void change_brightness(const QImage& src, QImage* dst, int dv){

    for(int y=0; y<src.height(); y++){
        const uint* src_line = (const uint*)src.constScanLine(y);
        uint* dst_line = (uint*)dst->scanLine(y);

        for(int x=0; x<src.width(); x++){
            uint rgb = src_line[x];

            int r = qRed(rgb);
            int g = qGreen(rgb);
            int b = qBlue(rgb);

            r = std::clamp(r + dv, 0, 255);
            g = std::clamp(g + dv, 0, 255);
            b = std::clamp(b + dv, 0, 255);

            dst_line[x] = qRgb(r, g, b);
        }
    }
}

void change_contrast(const QImage& src, QImage* dst, int dv) {

    double f = (259.0 * (dv + 255.0)) / (255.0 * (259.0 - dv));
    for(int y = 0; y < src.height(); y++) {
        const uint* src_line = (const uint*)src.constScanLine(y);
        uint* dst_line = (uint*)dst->scanLine(y);

        for(int x = 0; x < src.width(); x++) {
            uint rgb = src_line[x];

            int r = qRed(rgb);
            int g = qGreen(rgb);
            int b = qBlue(rgb);

            r = std::clamp((int)(f * (r - 128) + 128), 0, 255);
            g = std::clamp((int)(f * (g - 128) + 128), 0, 255);
            b = std::clamp((int)(f * (b - 128) + 128), 0, 255);

            dst_line[x] = qRgba(r, g, b, qAlpha(rgb));
        }
    }
}

void changed_gamma(const QImage& src, QImage* dst, double gamma){

    if(gamma <= 0.01) gamma = 0.01;

    for(int y=0; y<src.height(); y++){
        const uint* src_line = (const uint*)src.constScanLine(y);
        uint* dst_line = (uint*)dst->scanLine(y);

        for(int x=0; x<src.width(); x++){
            uint rgb = src_line[x];

            int r = std::clamp((int)(pow(qRed(rgb) / 255.0, gamma) * 255.0), 0, 255);
            int g = std::clamp((int)(pow(qGreen(rgb) / 255.0, gamma) * 255.0), 0, 255);
            int b = std::clamp((int)(pow(qBlue(rgb) / 255.0, gamma) * 255.0), 0, 255);

            dst_line[x] = qRgb(r, g, b);
        }
    }
}

void rgbZxyz(double r, double g, double b, double &x, double &y, double &z) {

    x = 0.4124 * r + 0.3576 * g + 0.1804 * b;
    y = 0.2126 * r + 0.7152 * g + 0.0722 * b;
    z = 0.0193 * r + 0.1192 * g + 0.9505 * b;
}


void xyzZlab(double x, double y, double z, double &L, double &a, double &b) {

    auto f = [](double t) { return (t > 0.008856) ? pow(t, 1.0/3.0) : (7.787 * t + 16.0/116.0); };
    L = 116.0 * f(y / 1.0000) - 16.0;
    a = 500.0 * (f(x / 0.9505) - f(y / 1.0000));
    b = 200.0 * (f(y / 1.0000) - f(z / 1.0891));
}

void labZxyz(double L, double a, double b, double &x, double &y, double &z) {

    auto f_inv = [](double t) { return (t > 0.206893) ? pow(t, 3.0) : (t - 16.0/116.0) / 7.787; };
    double fy = (L + 16.0) / 116.0;
    x = 0.9505 * f_inv(fy + (a / 500.0));
    y = 1.0000 * f_inv(fy);
    z = 1.0891 * f_inv(fy - (b / 200.0));
}

void xyzZrgb(double x, double y, double z, double &r, double &g, double &b) {

    r =  3.2406 * x - 1.5372 * y - 0.4986 * z;
    g = -0.9689 * x + 1.8758 * y + 0.0415 * z;
    b =  0.0557 * x - 0.2040 * y + 1.0570 * z;
}


void rgbZhsl(double r, double g, double b, double &h, double &s, double &l) {

    double maxV = std::max({r, g, b}), minV = std::min({r, g, b}), d = maxV - minV;
    l = (maxV + minV) / 2.0;
    if (d == 0) h = s = 0;
    else {
        s = l > 0.5 ? d / (2.0 - maxV - minV) : d / (maxV + minV);
        if (maxV == r) h = (g - b) / d + (g < b ? 6 : 0);
        else if (maxV == g) h = (b - r) / d + 2;
        else h = (r - g) / d + 4;
        h /= 6.0;
    }
}

void hslZrgb(double h, double s, double l, double &r, double &g, double &b) {

    if (s == 0) r = g = b = l;
    else {
        auto h2rgb = [](double p, double q, double t) {
            if (t < 0) t += 1; if (t > 1) t -= 1;
            if (t < 1.0/6.0) return p + (q - p) * 6 * t;
            if (t < 1.0/2.0) return q;
            if (t < 2.0/3.0) return p + (q - p) * (2.0/3.0 - t) * 6;
            return p;
        };
        double q = l < 0.5 ? l * (1 + s) : l + s - l * s, p = 2 * l - q;
        r = h2rgb(p, q, h + 1.0/3.0); g = h2rgb(p, q, h); b = h2rgb(p, q, h - 1.0/3.0);
    }
}

void histograms(const QImage& src, int* r, int* g, int* b) {

    for(int i=0; i<256; i++) {
        r[i] = g[i] = b[i] = 0;
    }

    for(int y=0; y<src.height(); y++) {
        for(int x=0; x<src.width(); x++) {
            QColor c = src.pixelColor(x, y);
            r[c.red()]++;
            g[c.green()]++;
            b[c.blue()]++;
        }
    }
}

void hsl(const QImage &src, QImage *dst, double dH, double dS, double dL) {

    for(int y=0; y<src.height(); y++) {
        for(int x=0; x<src.width(); x++) {
            QColor c = src.pixelColor(x, y);
            double r = c.redF(), g = c.greenF(), b = c.blueF();
            double h, s, l;

            rgbZhsl(r, g, b, h, s, l);

            h = std::fmod(h + (dH / 360.0), 1.0);
            if(h < 0) h += 1.0;
            s = std::clamp(s + dS, 0.0, 1.0);
            l = std::clamp(l + dL, 0.0, 1.0);
            hslZrgb(h, s, l, r, g, b);

            dst->setPixelColor(x, y, QColor::fromRgbF(
            std::clamp(r, 0.0, 1.0),
            std::clamp(g, 0.0, 1.0),
            std::clamp(b, 0.0, 1.0)
            ));
        }
    }
}

void lab(const QImage &src, QImage *dst, double dL, double da, double db) {

    for(int y=0; y<src.height(); y++) {
        for(int x=0; x<src.width(); x++) {
            QColor c = src.pixelColor(x, y);
            double r = c.redF(), g = c.greenF(), b = c.blueF();
            double X, Y, Z, L, a, b_val;

            rgbZxyz(r, g, b, X, Y, Z);
            xyzZlab(X, Y, Z, L, a, b_val);

            L = std::clamp(L + dL, 0.0, 100.0);
            a = std::clamp(a + da, -128.0, 127.0);
            b_val = std::clamp(b_val + db, -128.0, 127.0);

            labZxyz(L, a, b_val, X, Y, Z);
            xyzZrgb(X, Y, Z, r, g, b);

            dst->setPixelColor(x, y, QColor::fromRgbF(
            std::clamp(r, 0.0, 1.0),
            std::clamp(g, 0.0, 1.0),
            std::clamp(b, 0.0, 1.0)
            ));
        }
    }
}


void apply_convolution(const QImage& src, QImage* dst, const std::vector<std::vector<double>>& mask) {
    if (mask.empty() || mask[0].empty()) return;

    int mask_h = mask.size();
    int mask_w = mask[0].size();
    int offset_y = mask_h / 2;
    int offset_x = mask_w / 2;

    double weight_sum = 0.0;
    for (int i = 0; i < mask_h; i++) {
        for (int j = 0; j < mask_w; j++) {
            weight_sum += mask[i][j];
        }
    }
    if (weight_sum == 0.0) weight_sum = 1.0;

    for (int y = 0; y < src.height(); y++) {
        uint* dst_line = (uint*)dst->scanLine(y);

        for (int x = 0; x < src.width(); x++) {
            double r_acc = 0.0, g_acc = 0.0, b_acc = 0.0;

            for (int my = 0; my < mask_h; my++) {
                for (int mx = 0; mx < mask_w; mx++) {
                    int py = std::clamp(y + my - offset_y, 0, src.height() - 1);
                    int px = std::clamp(x + mx - offset_x, 0, src.width() - 1);

                    QRgb p = src.pixel(px, py);
                    double w = mask[my][mx];

                    r_acc += qRed(p) * w;
                    g_acc += qGreen(p) * w;
                    b_acc += qBlue(p) * w;
                }
            }

            int final_r = std::clamp((int)(r_acc / weight_sum), 0, 255);
            int final_g = std::clamp((int)(g_acc / weight_sum), 0, 255);
            int final_b = std::clamp((int)(b_acc / weight_sum), 0, 255);

            dst_line[x] = qRgba(final_r, final_g, final_b, qAlpha(src.pixel(x, y)));
        }
    }
}
