# imageinfo

Single header c++ library to get image size and format without loading/decoding. Super fast!

## Supported formats

* bmp
* gif
* hdr (pic)
* jpeg (jpg)
* png
* psd
* tga
* tiff (tif)
* webp
* more coming...

## Usage

### Simplest Demo

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

You can pass a file path and use `IIFilePathReader`, 

and there are some builtin reader `IIFileReader`, `IIFileStreamReader`, `IIRawDataReader`

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
// Suppose we already got data and size
// void *data;
// size_t size;
ImageInfo<IIRawData, IIRawDataReader> imageInfo(IIRawData(data, size));
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
ImageInfo<AAsset *, IIAndroidAssetFileReader> imageInfo(file);
AAsset_close(file);
```

Pretty easy?

Don't be stingy with your star : )
