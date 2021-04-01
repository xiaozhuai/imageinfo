# imageinfo

C++单个头文件库，在不加载/解码图片的情况下，获取图片文件类型和大小。性能极高。

## 支持格式

* bmp
* gif
* hdr (pic)
* jpeg (jpg)
* png
* psd
* tga
* tiff (tif)
* webp
* 更多...

## 用法

### 最简DEMO代码

```cpp
ImageInfo<const std::string &, IIFilePathReader> imageInfo("images/valid/jpg/sample.jpg");
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
ImageInfo<FILE *, IIFileReader> imageInfo(file);
fclose(file);
```

```cpp
std::ifstream file("images/valid/jpg/sample.jpg", std::ios::in);
ImageInfo<std::ifstream &, IIFileStreamReader> imageInfo(file);
file.close();
```

```cpp
// 假设已经得到了 data 和 size
// void *data;
// size_t size;
ImageInfo<IIRawData, IIRawDataReader> imageInfo(IIRawData(data, size));
```

如果你事先知道一个文件大概率是JPEG格式, 你可以提供额外的 `likely format` 参数来提升性能;

```cpp
ImageInfo<const std::string &, IIFilePathReader> imageInfo("images/valid/jpg/sample.jpg", II_FORMAT_JPEG);
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
ImageInfo<AAsset *, IIAndroidAssetFileReader> imageInfo(file);
AAsset_close(file);
```

很简单不是吗？

请不要吝啬你的Star : )
