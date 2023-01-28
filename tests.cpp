//
// Created by xiaozhuai on 2021/4/1.
//

#include <cstdio>
#include "imageinfo.hpp"

#define ASSET_II(file, error, format, width, height) do {                                                       \
    auto imageInfo = getImageInfo<IIFilePathReader>(file);                                                      \
    if (imageInfo.getErrorCode() != (error)) {                                                                  \
        fprintf(stderr, "Error ASSET_II, file: %s, line: %d, error != %s\n", file, __LINE__, #error);           \
        exit(1);                                                                                                \
    }                                                                                                           \
    else if (imageInfo.getFormat() != (format)) {                                                               \
        fprintf(stderr, "Error ASSET_II, file: %s, line: %d, format != %s\n", file, __LINE__, #format);         \
        exit(1);                                                                                                \
    }                                                                                                           \
    else if (imageInfo.getWidth() != (width)) {                                                                 \
        fprintf(stderr, "Error ASSET_II, file: %s, line: %d, width != %ld\n", file, __LINE__, (width));         \
        exit(1);                                                                                                \
    }                                                                                                           \
    else if (imageInfo.getHeight() != (height)) {                                                               \
        fprintf(stderr, "Error ASSET_II, file: %s, line: %d, height != %ld\n", file, __LINE__, (height));       \
        exit(1);                                                                                                \
    }                                                                                                           \
    else {                                                                                                      \
        printf("Test passed, file: %s \n", file);                                                               \
    }                                                                                                           \
} while(0)

int main() {
    {
        ASSET_II(IMAGES_DIRECTORY "valid/avif/sample.avif", II_ERR_OK, II_FORMAT_AVIF, 123l, 456l);
        ASSET_II(IMAGES_DIRECTORY "valid/avif/sample2.avif", II_ERR_OK, II_FORMAT_AVIF, 800l, 533l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/bmp/sample.bmp", II_ERR_OK, II_FORMAT_BMP, 123l, 456l);
        ASSET_II(IMAGES_DIRECTORY "valid/bmp/sample2.bmp", II_ERR_OK, II_FORMAT_BMP, 123l, 456l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/cur/sample.cur", II_ERR_OK, II_FORMAT_CUR, 32l, 32l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/dds/sample.dds", II_ERR_OK, II_FORMAT_DDS, 123l, 456l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/gif/sample.gif", II_ERR_OK, II_FORMAT_GIF, 123l, 456l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/hdr/sample.hdr", II_ERR_OK, II_FORMAT_HDR, 123l, 456l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/heic/sample.heic", II_ERR_OK, II_FORMAT_HEIC, 122l, 456l);
        ASSET_II(IMAGES_DIRECTORY "valid/heic/sample2.heic", II_ERR_OK, II_FORMAT_HEIC, 1440l, 960l);
        ASSET_II(IMAGES_DIRECTORY "valid/heic/sample3.heic", II_ERR_OK, II_FORMAT_HEIC, 1280l, 854l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/icns/sample.icns", II_ERR_OK, II_FORMAT_ICNS, 128l, 128l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/ico/multi-size.ico", II_ERR_OK, II_FORMAT_ICO, 256l, 256l);
        ASSET_II(IMAGES_DIRECTORY "valid/ico/multi-size-compressed.ico", II_ERR_OK, II_FORMAT_ICO, 256l, 256l);
        ASSET_II(IMAGES_DIRECTORY "valid/ico/sample.ico", II_ERR_OK, II_FORMAT_ICO, 32l, 32l);
        ASSET_II(IMAGES_DIRECTORY "valid/ico/sample-256.ico", II_ERR_OK, II_FORMAT_ICO, 256l, 256l);
        ASSET_II(IMAGES_DIRECTORY "valid/ico/sample-256-compressed.ico", II_ERR_OK, II_FORMAT_ICO, 256l, 256l);
        ASSET_II(IMAGES_DIRECTORY "valid/ico/sample-compressed.ico", II_ERR_OK, II_FORMAT_ICO, 32l, 32l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/jp2/sample.jp2", II_ERR_OK, II_FORMAT_JP2, 123l, 456l);
        ASSET_II(IMAGES_DIRECTORY "valid/jp2/jpx_disguised_as_jp2.jp2", II_ERR_OK, II_FORMAT_JP2, 2717l, 3701l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/jpg/1x2-flipped-big-endian.jpg", II_ERR_OK, II_FORMAT_JPEG, 1l, 2l);
        ASSET_II(IMAGES_DIRECTORY "valid/jpg/1x2-flipped-little-endian.jpg", II_ERR_OK, II_FORMAT_JPEG, 1l, 2l);
        ASSET_II(IMAGES_DIRECTORY "valid/jpg/large.jpg", II_ERR_OK, II_FORMAT_JPEG, 1600l, 1200l);
        ASSET_II(IMAGES_DIRECTORY "valid/jpg/optimized.jpg", II_ERR_OK, II_FORMAT_JPEG, 123l, 456l);
        ASSET_II(IMAGES_DIRECTORY "valid/jpg/progressive.jpg", II_ERR_OK, II_FORMAT_JPEG, 123l, 456l);
        ASSET_II(IMAGES_DIRECTORY "valid/jpg/sample.jpg", II_ERR_OK, II_FORMAT_JPEG, 123l, 456l);
        ASSET_II(IMAGES_DIRECTORY "valid/jpg/sampleExported.jpg", II_ERR_OK, II_FORMAT_JPEG, 123l, 456l);
        ASSET_II(IMAGES_DIRECTORY "valid/jpg/very-large.jpg", II_ERR_OK, II_FORMAT_JPEG, 4800l, 3600l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/jpx/sample.jpx", II_ERR_OK, II_FORMAT_JPX, 2717l, 3701l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/ktx/sample.ktx", II_ERR_OK, II_FORMAT_KTX, 123l, 456l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/png/sample.png", II_ERR_OK, II_FORMAT_PNG, 123l, 456l);
        ASSET_II(IMAGES_DIRECTORY "valid/png/sample_fried.png", II_ERR_OK, II_FORMAT_PNG, 128l, 68l);
        ASSET_II(IMAGES_DIRECTORY "valid/png/sample_apng.png", II_ERR_OK, II_FORMAT_PNG, 480l, 400l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/psd/sample.psd", II_ERR_OK, II_FORMAT_PSD, 123l, 456l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/qoi/sample.qoi", II_ERR_OK, II_FORMAT_QOI, 123l, 456l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/tga/sample.tga", II_ERR_OK, II_FORMAT_TGA, 123l, 456l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/tiff/big-endian.tiff", II_ERR_OK, II_FORMAT_TIFF, 123l, 456l);
        ASSET_II(IMAGES_DIRECTORY "valid/tiff/jpeg.tiff", II_ERR_OK, II_FORMAT_TIFF, 123l, 456l);
        ASSET_II(IMAGES_DIRECTORY "valid/tiff/little-endian.tiff", II_ERR_OK, II_FORMAT_TIFF, 123l, 456l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "valid/webp/lossless.webp", II_ERR_OK, II_FORMAT_WEBP, 123l, 456l);
        ASSET_II(IMAGES_DIRECTORY "valid/webp/extended.webp", II_ERR_OK, II_FORMAT_WEBP, 123l, 456l);
        ASSET_II(IMAGES_DIRECTORY "valid/webp/lossy.webp", II_ERR_OK, II_FORMAT_WEBP, 123l, 456l);
    }

    {
        ASSET_II(IMAGES_DIRECTORY "invalid/sample.png", II_ERR_UNRECOGNIZED_FORMAT, II_FORMAT_UNKNOWN, -1l, -1l);
    }

    return 0;
}