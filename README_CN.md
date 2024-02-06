# imageinfo

跨平台高性能的C++单个头文件库，在不加载/解码图片的情况下，获取图片文件类型和大小。

imageinfo 并不是通过扩展名来识别图片格式，而是通过文件头和文件格式特征来判断图片格式。

尽可能少的I/O次数！读取尽可能少的字节数！

部分测试图片文件来源于 [image-size](https://github.com/image-size/image-size) ，感谢 [@netroy](https://github.com/netroy)

Rust 版本: [imageinfo-rs](https://github.com/xiaozhuai/imageinfo-rs)

[![ci](https://github.com/xiaozhuai/imageinfo/actions/workflows/ci.yml/badge.svg)](https://github.com/xiaozhuai/imageinfo/actions/workflows/ci.yml)

imageinfo 已经重构，如果你使用的是旧版本，请查看 `v1` 分支。

## 支持格式

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
* [ ] 更多...

## Vcpkg

imageinfo 可以通过 [vcpkg](https://github.com/microsoft/vcpkg) 安装

```shell
vcpgk install imageinfo
```

## 构建 & 测试

### Linux & MacOS

```shell
cmake -B build .
cmake --build build -- all
cmake --build build -- check
```

### Windows

打开 Visual Studio Command Prompt 并执行下面的命令

```cmd
cmake -G "NMake Makefiles" -B build .
cmake --build build -- all
cmake --build build -- check
```

## 用法

### 最简DEMO代码

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

可以使用 `imageinfo::FilePathReader` 然后直接传入文件路径,

不同类型可以使用不同的 Reader, 如 `imageinfo::FileReader`, `imageinfo::FileStreamReader`, `imageinfo::RawDataReader`

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
// 假设已经得到了 data 和 size
// void *data;
// size_t size;
auto info = imageinfo::parse<imageinfo::RawDataReader>(imageinfo::RawData(data, size));
```

如果你事先知道一个文件大概率是JPEG格式, 你可以提供额外的 `likely_formats` 参数来提升性能;

```cpp
auto imageInfo = imageinfo::parse<imageinfo::FilePathReader>("images/valid/jpg/sample.jpg", {II_FORMAT_JPEG});
```

### 自定义Reader

首先，来看一下 `imageinfo::FileReader`, 要做的只是定义一个类，然后实现 `size` 和 `read` 方法。(非override)

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

然后，让我们来尝试实现一个Android assets文件的Reader

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
// 假设已经得到了 AAssetManager
// AAssetManager *manager;
// 以 AASSET_MODE_RANDOM 模式打开以支持双向seek
AAsset *file = AAssetManager_open(manager, "test.png", AASSET_MODE_RANDOM);
auto imageInfo = imageinfo::parse<imageinfo::AndroidAssetFileReader>(file);
AAsset_close(file);
```

很简单不是吗？

请不要吝啬你的Star : )
