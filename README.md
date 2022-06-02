# imageinfo

Cross platform super fast single header c++ library to get image size and format without loading/decoding.

The imageinfo don't get image format by file ext name, but infer by file header bytes and character.

As few I/O times as possible! Read as few bytes as possible!

Some test image files are from [image-size](https://github.com/image-size/image-size). Many thanks to [@netroy](https://github.com/netroy).

Rust version: [imageinfo-rs](https://github.com/xiaozhuai/imageinfo-rs)

[![linux](https://github.com/xiaozhuai/imageinfo/actions/workflows/linux.yml/badge.svg)](https://github.com/xiaozhuai/imageinfo/actions/workflows/linux.yml)
[![macos](https://github.com/xiaozhuai/imageinfo/actions/workflows/macos.yml/badge.svg)](https://github.com/xiaozhuai/imageinfo/actions/workflows/macos.yml)
[![windows-x64](https://github.com/xiaozhuai/imageinfo/actions/workflows/windows-x64.yml/badge.svg)](https://github.com/xiaozhuai/imageinfo/actions/workflows/windows-x64.yml)
[![windows-x86](https://github.com/xiaozhuai/imageinfo/actions/workflows/windows-x86.yml/badge.svg)](https://github.com/xiaozhuai/imageinfo/actions/workflows/windows-x86.yml)

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

## Build & Test

### Linux & MacOS

```shell
cmake -B build .
cmake --build build -- all
cmake --build build -- check
```

### Windows

```cmd
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars32.bat"
cmake -G "NMake Makefiles" -B build .
cmake --build build -- all
cmake --build build -- check
```

## Usage

### Simplest Demo

```cpp
auto imageInfo = getImageInfo<IIFilePathReader>("images/valid/jpg/sample.jpg");
std::cout << "File: " << file << "\n";
std::cout << "  - Error    : " << imageInfo.getErrorMsg() << "\n";
std::cout << "  - Width    : " << imageInfo.getWidth() << "\n";
std::cout << "  - Height   : " << imageInfo.getHeight() << "\n";
std::cout << "  - Format   : " << imageInfo.getFormat() << "\n";
std::cout << "  - Ext      : " << imageInfo.getExt() << "\n";
std::cout << "  - Full Ext : " << imageInfo.getFullExt() << "\n";
std::cout << "  - Mimetype : " << imageInfo.getMimetype() << "\n\n";
```

You can pass a file path and use `IIFilePathReader`, 

and there are some builtin reader `IIFileReader`, `IIFileStreamReader`, `IIRawDataReader`

```cpp
FILE *file = fopen("images/valid/jpg/sample.jpg", "rb");
auto imageInfo = getImageInfo<IIFileReader>(file);
fclose(file);
```

```cpp
std::ifstream file("images/valid/jpg/sample.jpg", std::ios::in);
auto imageInfo = getImageInfo<IIFileStreamReader>(file);
file.close();
```

```cpp
// Suppose we already got data and size
// void *data;
// size_t size;
auto imageInfo = getImageInfo<IIRawDataReader>(IIRawData(data, size));
```

If you known the file is likely a JPEG, you can provide `likely format` parameter to improve performance;

```cpp
auto imageInfo = getImageInfo<IIFilePathReader>("images/valid/jpg/sample.jpg", II_FORMAT_JPEG);
```

### Custom Reader

First, take a look at `IIFileReader`, all your need to do is define a class and implement `size` and `read` method. (not override)

```cpp
class IIFileReader {
public:
    explicit IIFileReader(FILE *file) : m_file(file) {}

    inline size_t size() {
        if (m_file != nullptr) {
            fseek(m_file, 0, SEEK_END);
            return ftell(m_file);
        } else {
            return 0;
        }
    }

    inline void read(void *buf, off_t offset, size_t size) {
        fseek(m_file, offset, SEEK_SET);
        fread(buf, 1, size, m_file);
    }

private:
    FILE *m_file = nullptr;
};
```

Then, let's try to make a reader for Android assets file

```cpp
class IIAndroidAssetFileReader {
public:
    explicit IIAndroidAssetFileReader(AAsset *file) : m_file(file) {}

    inline size_t size() {
        if (m_file != nullptr) {
            return AAsset_getLength(m_file);
        } else {
            return 0;
        }
    }

    inline void read(void *buf, off_t offset, size_t size) {
        AAsset_seek(m_file, offset, SEEK_SET);
        AAsset_read(m_file, buf, size);
    }

private:
    AAsset *m_file = nullptr;
};
```

```cpp
// Suppose we have a AAssetManager
// AAssetManager *manager;
// Open with AASSET_MODE_RANDOM mode to seek forward and backward
AAsset *file = AAssetManager_open(manager, "test.png", AASSET_MODE_RANDOM);
auto imageInfo = getImageInfo<IIAndroidAssetFileReader>(file);
AAsset_close(file);
```

Pretty easy?

Don't be stingy with your star : )
