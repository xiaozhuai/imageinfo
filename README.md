# imageinfo

Cross platform super fast single header c++ library to get image size and format without loading/decoding.

The imageinfo don't get image format by file ext name, but infer by file header bytes and character.

As few I/O times as possible! Read as few bytes as possible!

Some test image files are from [image-size](https://github.com/image-size/image-size). Many thanks to [@netroy](https://github.com/netroy).

Rust version: [imageinfo-rs](https://github.com/xiaozhuai/imageinfo-rs)

[![ci](https://github.com/xiaozhuai/imageinfo/actions/workflows/ci.yml/badge.svg)](https://github.com/xiaozhuai/imageinfo/actions/workflows/ci.yml)

imageinfo has been restructured, if you are using old version, please check `v1` branch.

## Supported formats

* [x] avif
* [x] bmp
* [x] cur
* [x] dds
* [x] gif
* [x] hdr (pic)
* [x] heic (heif)
* [x] icns
* [x] ico
* [x] jp2
* [x] jpeg (jpg)
* [x] jpx
* [x] ktx
* [x] png
* [x] psd
* [x] qoi
* [ ] svg
* [x] tga
* [x] tiff (tif)
* [x] webp
* [ ] more coming...

## Vcpkg

imageinfo is available on [vcpkg](https://github.com/microsoft/vcpkg)

```shell
vcpgk install imageinfo
```

## Build & Test

### Linux & MacOS

```shell
cmake -B build .
cmake --build build -- all
cmake --build build -- check
```

### Windows

Open Visual Studio Command Prompt and run these command

```cmd
cmake -G "NMake Makefiles" -B build .
cmake --build build -- all
cmake --build build -- check
```

## Usage

### Simplest Demo

```cpp
const char *file = "images/valid/jpg/sample.jpg";
auto info = imageinfo::parse<imageinfo::FilePathReader>(file);
std::cout << "File: " << file << "\n";
std::cout << "  - Error    : " << info.error_msg() << "\n";
std::cout << "  - Width    : " << info.size().width << "\n";
std::cout << "  - Height   : " << info.size().height << "\n";
std::cout << "  - Format   : " << info.format() << "\n";
std::cout << "  - Ext      : " << info.ext() << "\n";
std::cout << "  - Full Ext : " << info.full_ext() << "\n";
std::cout << "  - Mimetype : " << info.mimetype() << "\n\n";
```

You can pass a file path and use `imageinfo::FilePathReader`, 

and there are some builtin reader `imageinfo::FileReader`, `imageinfo::FileStreamReader`, `imageinfo::RawDataReader`

```cpp
FILE *file = fopen("images/valid/jpg/sample.jpg", "rb");
auto info = imageinfo::parse<imageinfo::FileReader>(file);
fclose(file);
```

```cpp
std::ifstream file("images/valid/jpg/sample.jpg", std::ios::in | std::ios::binary);
auto info = imageinfo::parse<imageinfo::FileStreamReader>(file);
file.close();
```

```cpp
// Suppose we already got data and size
// void *data;
// size_t size;
auto info = imageinfo::parse<imageinfo::RawDataReader>(imageinfo::RawData(data, size));
```

If you known the file is likely a JPEG, you can provide `likely_formats` parameter to improve performance;

```cpp
auto imageInfo = imageinfo::parse<imageinfo::FilePathReader>("images/valid/jpg/sample.jpg", {II_FORMAT_JPEG});
```

### Custom Reader

First, take a look at `imageinfo::FileReader`, all your need to do is define a class and implement `size` and `read` method. (not override)

```cpp
class FileReader {
public:
    explicit FileReader(FILE *file) : file_(file) {}

    inline size_t size() {
        if (file_ != nullptr) {
            fseek(file_, 0, SEEK_END);
            return ftell(file_);
        } else {
            return 0;
        }
    }

    inline void read(void *buf, off_t offset, size_t size) {
        fseek(file_, offset, SEEK_SET);
        fread(buf, 1, size, file_);
    }

private:
    FILE *file_ = nullptr;
};
```

Then, let's try to make a reader for Android assets file

```cpp
class AndroidAssetFileReader {
public:
    explicit AndroidAssetFileReader(AAsset *file) : file_(file) {}

    inline size_t size() {
        if (file_ != nullptr) {
            return AAsset_getLength(file_);
        } else {
            return 0;
        }
    }

    inline void read(void *buf, off_t offset, size_t size) {
        AAsset_seek(file_, offset, SEEK_SET);
        AAsset_read(file_, buf, size);
    }

private:
    AAsset *file_ = nullptr;
};
```

```cpp
// Suppose we have a AAssetManager
// AAssetManager *manager;
// Open with AASSET_MODE_RANDOM mode to seek forward and backward
AAsset *file = AAssetManager_open(manager, "test.png", AASSET_MODE_RANDOM);
auto imageInfo = imageinfo::parse<imageinfo::AndroidAssetFileReader>(file);
AAsset_close(file);
```

Pretty easy?

Don't be stingy with your star : )
