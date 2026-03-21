#include "image_algorithm.h"


void change_brightness(const QImage& src, QImage* dst, int dv){
    for(int y=0; y<src.height(); y++){
        uint* src_rgb = (uint*)src.scanLine(y);
        uint* dst_rgb = (uint*)dst->scanLine(y);
        for(int x=0; x<src.width(); x++){
            uint* p = (uint*)src.bits();
            uint rgb = p[y*src.width() + x];

            uchar r = ((rgb>>16) & 0xff);
            uchar g = ((rgb>>8) & 0xff);
            uchar b = (rgb & 0xff);
            r = std::clamp(r+dv, 0, 255);
            g = std::clamp(g+dv, 0, 255);
            b = std::clamp(b+dv, 0, 255);
            rgb = (r<<16) + (g<<8) + b + 0xff000000;
            dst_rgb[x] = rgb;
        }
    }
}

void change_contrast(const QImage& src, QImage* dst, int dv){
    int v = std::clamp(dv, -126, 126);
    double contrast = (127.0 + v) / (127.0 - v);
    for(int y=0; y<src.height(); y++){
        uint* src_rgb = (uint*)src.scanLine(y);
        uint* dst_rgb = (uint*)dst->scanLine(y);
        for(int x=0; x<src.width(); x++){
            uint* p = (uint*)src.bits();
            uint rgb = p[y*src.width() + x];

            uchar r = ((rgb>>16) & 0xff);
            uchar g = ((rgb>>8) & 0xff);
            uchar b = (rgb & 0xff);
            r = std::clamp((int)(contrast*(r-128) + 128), 0, 255);
            g = std::clamp((int)(contrast*(g-128) + 128), 0, 255);
            b = std::clamp((int)(contrast*(b-128) + 128), 0, 255);
            rgb = (r<<16) + (g<<8) + b + 0xff000000;
            dst_rgb[x] = rgb;
        }
    }
}


void changed_gamma(const QImage& src, QImage* dst, double dv){
    if(dv <= 0) dv = 0.01;
    for(int y=0; y<src.height(); y++){
        const uint* src_rgb = (const uint*)src.scanLine(y);
        uint* dst_rgb = (uint*)dst->scanLine(y);
        for(int x=0; x<src.width(); x++){
            uint* p = (uint*)src.bits();
            uint rgb = p[y*src.width() + x];
            // uint rgb = src_rgb[x];

            double r = ((rgb>>16) & 0xff) / 255.0;
            double g = ((rgb>>8) & 0xff) / 255.0;
            double b = (rgb & 0xff) / 255.0;
            int r_i = std::clamp((int)(pow(r, dv) * 255.0 + 0.5), 0, 255);
            int g_i = std::clamp((int)(pow(g, dv) * 255.0 + 0.5), 0, 255);
            int b_i = std::clamp((int)(pow(b, dv) * 255.0 + 0.5), 0, 255);
            dst_rgb[x] = (r_i<<16) + (g_i<<8) + b_i + 0xff000000;
        }
    }
}

void histograms(const QImage& src, int* r, int* g, int* b) {
    for(int i=0; i<256; i++) {
        r[i] = 0;
        g[i] = 0;
        b[i] = 0;
    }

    for(int y=0; y<src.height(); y++) {
        const uint* line = (const uint*)src.constScanLine(y);
        for(int x=0; x<src.width(); x++) {
            uint rgb = line[x];
            r[(rgb >> 16) & 0xff]++;
            g[(rgb >> 8) & 0xff]++;
            b[rgb & 0xff]++;
        }
    }
}

