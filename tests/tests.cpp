//
// Created by xiaozhuai on 2021/4/1.
//

#include <cstdio>

#include "imageinfo.hpp"

#define ASSET_II(file, e, f, w, h)                                                                       \
    do {                                                                                                 \
        auto info = imageinfo::parse<imageinfo::FilePathReader>(file);                                   \
        if (info.error() != (e)) {                                                                       \
            fprintf(stderr, "Error ASSET_II, file: %s, line: %d, error != %s, %s\n", file, __LINE__, #e, \
                    info.error_msg());                                                                   \
            abort();                                                                                     \
        } else if (info.format() != (f)) {                                                               \
            fprintf(stderr, "Error ASSET_II, file: %s, line: %d, format != %s\n", file, __LINE__, #f);   \
            abort();                                                                                     \
        } else if (info.size().width != (w)) {                                                           \
            fprintf(stderr, "Error ASSET_II, file: %s, line: %d, width != %ld\n", file, __LINE__, (w));  \
            abort();                                                                                     \
        } else if (info.size().height != (h)) {                                                          \
            fprintf(stderr, "Error ASSET_II, file: %s, line: %d, height != %ld\n", file, __LINE__, (h)); \
            abort();                                                                                     \
        } else {                                                                                         \
            printf("Test passed, file: %s \n", file);                                                    \
        }                                                                                                \
    } while (0)

int main() {
    using namespace imageinfo;

    {
        ASSET_II(IMAGES_DIR "valid/avif/sample.avif", kNoError, kFormatAvif, 123l, 456l);
        ASSET_II(IMAGES_DIR "valid/avif/sample2.avif", kNoError, kFormatAvif, 800l, 533l);
        ASSET_II(IMAGES_DIR "valid/avif/sample3.avif", kNoError, kFormatAvif, 1280l, 720l);
    }

    {
        ASSET_II(IMAGES_DIR "valid/heic/sample.heic", kNoError, kFormatHeic, 122l, 456l);
        ASSET_II(IMAGES_DIR "valid/heic/sample2.heic", kNoError, kFormatHeic, 1440l, 960l);
        ASSET_II(IMAGES_DIR "valid/heic/sample3.heic", kNoError, kFormatHeic, 1280l, 854l);
    }

    {
        ASSET_II(IMAGES_DIR "valid/bmp/sample.bmp", kNoError, kFormatBmp, 123l, 456l);
        ASSET_II(IMAGES_DIR "valid/bmp/sample2.bmp", kNoError, kFormatBmp, 123l, 456l);
    }

    { ASSET_II(IMAGES_DIR "valid/cur/sample.cur", kNoError, kFormatCur, 32l, 32l); }

    {
        ASSET_II(IMAGES_DIR "valid/ico/multi-size.ico", kNoError, kFormatIco, 256l, 256l);
        ASSET_II(IMAGES_DIR "valid/ico/multi-size-compressed.ico", kNoError, kFormatIco, 256l, 256l);
        ASSET_II(IMAGES_DIR "valid/ico/sample.ico", kNoError, kFormatIco, 32l, 32l);
        ASSET_II(IMAGES_DIR "valid/ico/sample-256.ico", kNoError, kFormatIco, 256l, 256l);
        ASSET_II(IMAGES_DIR "valid/ico/sample-256-compressed.ico", kNoError, kFormatIco, 256l, 256l);
        ASSET_II(IMAGES_DIR "valid/ico/sample-compressed.ico", kNoError, kFormatIco, 32l, 32l);
    }

    { ASSET_II(IMAGES_DIR "valid/dds/sample.dds", kNoError, kFormatDds, 123l, 456l); }

    { ASSET_II(IMAGES_DIR "valid/gif/sample.gif", kNoError, kFormatGif, 123l, 456l); }

    {
        ASSET_II(IMAGES_DIR "valid/hdr/sample.hdr", kNoError, kFormatHdr, 123l, 456l);
        ASSET_II(IMAGES_DIR "valid/hdr/sample2.hdr", kNoError, kFormatHdr, 1024l, 512l);
    }

    { ASSET_II(IMAGES_DIR "valid/icns/sample.icns", kNoError, kFormatIcns, 128l, 128l); }

    {
        ASSET_II(IMAGES_DIR "valid/jp2/sample.jp2", kNoError, kFormatJp2, 123l, 456l);
        ASSET_II(IMAGES_DIR "valid/jp2/jpx_disguised_as_jp2.jp2", kNoError, kFormatJp2, 2717l, 3701l);
    }

    { ASSET_II(IMAGES_DIR "valid/jpx/sample.jpx", kNoError, kFormatJpx, 2717l, 3701l); }

    {
        ASSET_II(IMAGES_DIR "valid/jpg/1x2-flipped-big-endian.jpg", kNoError, kFormatJpeg, 1l, 2l);
        ASSET_II(IMAGES_DIR "valid/jpg/1x2-flipped-little-endian.jpg", kNoError, kFormatJpeg, 1l, 2l);
        ASSET_II(IMAGES_DIR "valid/jpg/large.jpg", kNoError, kFormatJpeg, 1600l, 1200l);
        ASSET_II(IMAGES_DIR "valid/jpg/optimized.jpg", kNoError, kFormatJpeg, 123l, 456l);
        ASSET_II(IMAGES_DIR "valid/jpg/progressive.jpg", kNoError, kFormatJpeg, 123l, 456l);
        ASSET_II(IMAGES_DIR "valid/jpg/sample.jpg", kNoError, kFormatJpeg, 123l, 456l);
        ASSET_II(IMAGES_DIR "valid/jpg/sample2.jpg", kNoError, kFormatJpeg, 1200l, 1603l);
        ASSET_II(IMAGES_DIR "valid/jpg/sampleExported.jpg", kNoError, kFormatJpeg, 123l, 456l);
        ASSET_II(IMAGES_DIR "valid/jpg/very-large.jpg", kNoError, kFormatJpeg, 4800l, 3600l);
    }

    { ASSET_II(IMAGES_DIR "valid/ktx/sample.ktx", kNoError, kFormatKtx, 123l, 456l); }

    {
        ASSET_II(IMAGES_DIR "valid/png/sample.png", kNoError, kFormatPng, 123l, 456l);
        ASSET_II(IMAGES_DIR "valid/png/sample_fried.png", kNoError, kFormatPng, 128l, 68l);
        ASSET_II(IMAGES_DIR "valid/png/sample_apng.png", kNoError, kFormatPng, 480l, 400l);
        ASSET_II(IMAGES_DIR "invalid/sample.png", kUnrecognizedFormat, kFormatUnknown, -1l, -1l);
    }

    { ASSET_II(IMAGES_DIR "valid/psd/sample.psd", kNoError, kFormatPsd, 123l, 456l); }

    { ASSET_II(IMAGES_DIR "valid/qoi/sample.qoi", kNoError, kFormatQoi, 123l, 456l); }

    {
        ASSET_II(IMAGES_DIR "valid/tiff/big-endian.tiff", kNoError, kFormatTiff, 123l, 456l);
        ASSET_II(IMAGES_DIR "valid/tiff/jpeg.tiff", kNoError, kFormatTiff, 123l, 456l);
        ASSET_II(IMAGES_DIR "valid/tiff/little-endian.tiff", kNoError, kFormatTiff, 123l, 456l);
    }

    {
        ASSET_II(IMAGES_DIR "valid/webp/lossless.webp", kNoError, kFormatWebp, 123l, 456l);
        ASSET_II(IMAGES_DIR "valid/webp/extended.webp", kNoError, kFormatWebp, 123l, 456l);
        ASSET_II(IMAGES_DIR "valid/webp/lossy.webp", kNoError, kFormatWebp, 123l, 456l);
    }

    { ASSET_II(IMAGES_DIR "valid/tga/sample.tga", kNoError, kFormatTga, 123l, 456l); }

    {
        // These files are from @chan233
        // https://github.com/xiaozhuai/imageinfo/pull/14
        // https://github.com/xiaozhuai/imageinfo/pull/15
        ASSET_II(IMAGES_DIR "invalid/crash_assert_failed", kUnrecognizedFormat, kFormatUnknown, -1l, -1l);
        ASSET_II(IMAGES_DIR "invalid/crash_avif_1", kUnrecognizedFormat, kFormatUnknown, -1l, -1l);
        ASSET_II(IMAGES_DIR "invalid/crash_avif_2", kUnrecognizedFormat, kFormatUnknown, -1l, -1l);
        ASSET_II(IMAGES_DIR "invalid/crash_hdr_1", kUnrecognizedFormat, kFormatUnknown, -1l, -1l);
        ASSET_II(IMAGES_DIR "invalid/crash_hdr_2", kUnrecognizedFormat, kFormatUnknown, -1l, -1l);
        ASSET_II(IMAGES_DIR "invalid/crash_hdr_3", kUnrecognizedFormat, kFormatUnknown, -1l, -1l);
        ASSET_II(IMAGES_DIR "invalid/crash_hdr_4", kUnrecognizedFormat, kFormatUnknown, -1l, -1l);
        ASSET_II(IMAGES_DIR "invalid/crash_png_1", kUnrecognizedFormat, kFormatUnknown, -1l, -1l);
        ASSET_II(IMAGES_DIR "invalid/crash_png_2", kUnrecognizedFormat, kFormatUnknown, -1l, -1l);
        ASSET_II(IMAGES_DIR "invalid/crash_icns_1", kUnrecognizedFormat, kFormatUnknown, -1l, -1l);
        ASSET_II(IMAGES_DIR "invalid/crash_icns_2", kUnrecognizedFormat, kFormatUnknown, -1l, -1l);
        ASSET_II(IMAGES_DIR "invalid/crash_tiff_1", kUnrecognizedFormat, kFormatUnknown, -1l, -1l);
    }

    return 0;
}
