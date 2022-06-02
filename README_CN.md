# imageinfo

跨平台高性能的C++单个头文件库，在不加载/解码图片的情况下，获取图片文件类型和大小。

imageinfo 并不是通过扩展名来识别图片格式，而是通过文件头和文件格式特征来判断图片格式。

尽可能少的I/O次数！读取尽可能少的字节数！

部分测试图片文件来源于 [image-size](https://github.com/image-size/image-size) ，感谢 [@netroy](https://github.com/netroy)

Rust 版本: [imageinfo-rs](https://github.com/xiaozhuai/imageinfo-rs)

[![linux](https://github.com/xiaozhuai/imageinfo/actions/workflows/linux.yml/badge.svg)](https://github.com/xiaozhuai/imageinfo/actions/workflows/linux.yml)
[![macos](https://github.com/xiaozhuai/imageinfo/actions/workflows/macos.yml/badge.svg)](https://github.com/xiaozhuai/imageinfo/actions/workflows/macos.yml)
[![windows-x64](https://github.com/xiaozhuai/imageinfo/actions/workflows/windows-x64.yml/badge.svg)](https://github.com/xiaozhuai/imageinfo/actions/workflows/windows-x64.yml)
[![windows-x86](https://github.com/xiaozhuai/imageinfo/actions/workflows/windows-x86.yml/badge.svg)](https://github.com/xiaozhuai/imageinfo/actions/workflows/windows-x86.yml)

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

## 构建 & 测试

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

## 用法

### 最简DEMO代码

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

可以使用 `IIFilePathReader` 然后直接传入文件路径,

不同类型可以使用不同的 Reader, 如 `IIFileReader`, `IIFileStreamReader`, `IIRawDataReader`

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
// 假设已经得到了 data 和 size
// void *data;
// size_t size;
auto imageInfo = getImageInfo<IIRawDataReader>(IIRawData(data, size));
```

如果你事先知道一个文件大概率是JPEG格式, 你可以提供额外的 `likely format` 参数来提升性能;

```cpp
auto imageInfo = getImageInfo<IIFilePathReader>("images/valid/jpg/sample.jpg", II_FORMAT_JPEG);
```

### 自定义Reader

首先，来看一下 `IIFileReader`, 要做的只是定义一个类，然后实现 `size` 和 `read` 方法。(非override)

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

然后，让我们来尝试实现一个Android assets文件的Reader

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
// 假设已经得到了 AAssetManager
// AAssetManager *manager;
// 以 AASSET_MODE_RANDOM 模式打开以支持双向seek
AAsset *file = AAssetManager_open(manager, "test.png", AASSET_MODE_RANDOM);
auto imageInfo = getImageInfo<IIAndroidAssetFileReader>(file);
AAsset_close(file);
```

很简单不是吗？

请不要吝啬你的Star : )
