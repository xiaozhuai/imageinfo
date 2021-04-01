//
// Created by xiaozhuai on 2021/4/1.
//

#pragma once
#ifndef IMAGEINFO_IMAGEINFO_H
#define IMAGEINFO_IMAGEINFO_H

#include <functional>
#include <algorithm>
#include <fstream>
#include <string>
#include <regex>
#include <utility>
#include <vector>
#include <array>
#include <cstdio>

template<typename T>
inline T __ii_swap_endian(T u) {
    static_assert(sizeof(uint8_t) == 1, "CHAR_BIT != 8");
    union {
        T u;
        uint8_t u8[sizeof(T)];
    } source, dest;
    source.u = u;
    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];
    return dest.u;
}

enum IIFormat {
    II_FORMAT_UNKNOWN = 0,
    II_FORMAT_BMP,
    II_FORMAT_GIF,
    II_FORMAT_HDR,
    II_FORMAT_JPEG,
    II_FORMAT_PNG,
    II_FORMAT_PSD,
    II_FORMAT_TGA,
    II_FORMAT_TIFF,
    II_FORMAT_WEBP,
};

enum IIErrorCode {
    II_ERR_OK = 0,
    II_ERR_UNRECOGNIZED_FORMAT,
    II_ERR_DECODE_SIZE_FAILED,
};

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

class IIFilePathReader {
public:
    explicit IIFilePathReader(const std::string &path) : m_file(path, std::ios::in | std::ios::binary) {}

    ~IIFilePathReader() {
        if (m_file.is_open()) {
            m_file.close();
        }
    }

    inline size_t size() {
        if (m_file.is_open()) {
            m_file.seekg(0, std::ios::end);
            return m_file.tellg();
        } else {
            return 0;
        }
    }

    inline void read(void *buf, off_t offset, size_t size) {
        m_file.seekg(offset, std::ios::beg);
        m_file.read((char *) buf, size);
    }

private:
    std::ifstream m_file;
};

class IIFileStreamReader {
public:
    explicit IIFileStreamReader(std::ifstream &file) : m_file(file) {}

    explicit IIFileStreamReader(std::ifstream *file) : m_file(*file) {}

    inline size_t size() {
        if (m_file.is_open()) {
            m_file.seekg(0, std::ios::end);
            return m_file.tellg();
        } else {
            return 0;
        }
    }

    inline void read(void *buf, off_t offset, size_t size) {
        m_file.seekg(offset, std::ios::beg);
        m_file.read((char *) buf, size);
    }

private:
    std::ifstream &m_file;
};

struct IIRawData {
    IIRawData(const void *d, size_t s) : data(d), length(s) {}

    const void *data = nullptr;
    size_t length = 0;
};

class IIRawDataReader {
public:
    explicit IIRawDataReader(IIRawData data) : m_data(data) {}

    inline size_t size() const {
        return m_data.length;
    }

    inline void read(void *buf, off_t offset, size_t size) const {
        memcpy(buf, ((char *) m_data.data) + offset, size);
    }

private:
    IIRawData m_data;
};

typedef std::function<void(void *buf, off_t offset, size_t size)> IIReadFunc;

class IIReadInterface {
public:
    IIReadInterface() = delete;

    explicit IIReadInterface(IIReadFunc &r) : read(r) {}

public:
    inline uint8_t readU8(off_t offset) {
        uint8_t val;
        read(&val, offset, sizeof(uint8_t));
        return val;
    }

    inline int8_t readS8(off_t offset) {
        int8_t val;
        read(&val, offset, sizeof(int8_t));
        return val;
    }

    inline uint16_t readU16LE(off_t offset) {
        uint16_t val;
        read(&val, offset, sizeof(uint16_t));
        return val;
    }

    inline uint16_t readU16BE(off_t offset) {
        uint16_t val;
        read(&val, offset, sizeof(uint16_t));
        val = __ii_swap_endian<uint16_t>(val);
        return val;
    }

    inline int16_t readS16LE(off_t offset) {
        int16_t val;
        read(&val, offset, sizeof(int16_t));
        return val;
    }

    inline int16_t readS16BE(off_t offset) {
        int16_t val;
        read(&val, offset, sizeof(int16_t));
        val = __ii_swap_endian<int16_t>(val);
        return val;
    }

    inline uint32_t readU32LE(off_t offset) {
        uint32_t val;
        read(&val, offset, sizeof(uint32_t));
        return val;
    }

    inline uint32_t readU32BE(off_t offset) {
        uint32_t val;
        read(&val, offset, sizeof(uint32_t));
        val = __ii_swap_endian<uint32_t>(val);
        return val;
    }

    inline int32_t readS32LE(off_t offset) {
        int32_t val;
        read(&val, offset, sizeof(int32_t));
        return val;
    }

    inline int32_t readS32BE(off_t offset) {
        int32_t val;
        read(&val, offset, sizeof(int32_t));
        val = __ii_swap_endian<int32_t>(val);
        return val;
    }

//    inline uint64_t readU64LE(off_t offset) {
//        uint64_t val;
//        read(&val, offset, sizeof(uint64_t));
//        return val;
//    }
//
//    inline uint64_t readU64BE(off_t offset) {
//        uint64_t val;
//        read(&val, offset, sizeof(uint64_t));
//        val = __ii_swap_endian<uint64_t>(val);
//        return val;
//    }
//
//    inline int64_t readS64LE(off_t offset) {
//        int64_t val;
//        read(&val, offset, sizeof(int64_t));
//        return val;
//    }
//
//    inline int64_t readS64BE(off_t offset) {
//        int64_t val;
//        read(&val, offset, sizeof(int64_t));
//        val = __ii_swap_endian<int64_t>(val);
//        return val;
//    }

    inline std::string readString(off_t offset, size_t size) {
        std::string str(size, 0);
        read((void *) str.data(), offset, size);
        return str;
    }

    inline bool cmp(off_t offset, size_t size, const void *mem) {
        std::vector<uint8_t> m1(size);
        read(m1.data(), offset, size);
        return memcmp(m1.data(), mem, size) == 0;
    }

    inline bool cmpOneOf(off_t offset, size_t size, const std::initializer_list<const void *> &mems) {
        std::vector<uint8_t> m1(size);
        read(m1.data(), offset, size);
        for (auto *mem : mems) {
            if (memcmp(m1.data(), mem, size) == 0) return true;
        }
        return false;
    }

private:
    IIReadFunc &read;
};

typedef std::function<void(size_t length, IIReadInterface &ri, bool &match, int64_t &width,
                           int64_t &height)> IIProcessFunc;

struct IIDetector {
    IIDetector(IIFormat f, const char *e, const char *fe, const char *mt, IIProcessFunc p)
            : format(f),
              ext(e),
              fullExt(fe),
              mimetype(mt),
              process(std::move(p)) {}

    IIFormat format;
    const char *ext;
    const char *fullExt;
    const char *mimetype;
    IIProcessFunc process;
};

template<typename T1, typename T2>
class ImageInfo {
public:
    ImageInfo() = delete;

    explicit ImageInfo(T1 file) {
        T2 fileReader(file);
        IIReadFunc read = [&](void *buf, off_t offset, size_t size) { fileReader.read(buf, offset, size); };
        IIReadInterface ri(read);
        auto detectors = getDetectors();
        size_t length = fileReader.size();
        bool match;
        for (auto &detector : detectors) {
            detector.process(length, ri, match, m_width, m_height);
            if (match) {
                m_format = detector.format;
                m_ext = detector.ext;
                m_fullExt = detector.fullExt;
                m_mimetype = detector.mimetype;
                m_err = m_width != -1 && m_height != -1
                        ? II_ERR_OK
                        : II_ERR_DECODE_SIZE_FAILED;
                return;
            }
        }
        m_err = II_ERR_UNRECOGNIZED_FORMAT;
    }

    inline explicit operator bool() const {
        return m_err == II_ERR_OK;
    }

    inline bool ok() const {
        return m_err == II_ERR_OK;
    }

    inline IIFormat getFormat() const {
        return m_format;
    }

    inline const char *getExt() const {
        return m_ext;
    }

    inline const char *getFullExt() const {
        return m_fullExt;
    }

    inline const char *getMimetype() const {
        return m_mimetype;
    }

    inline int64_t getWidth() const {
        return m_width;
    }

    inline int64_t getHeight() const {
        return m_height;
    }

    inline std::array<int64_t, 2> getSize() const {
        return {m_width, m_height};
    }

    inline IIErrorCode getErrorCode() const {
        return m_err;
    }

    inline const char *getErrorMsg() const {
        switch (m_err) {
            default:
                return "Unknown error";
            case II_ERR_OK:
                return "Ok";
            case II_ERR_UNRECOGNIZED_FORMAT:
                return "Unrecognized format";
            case II_ERR_DECODE_SIZE_FAILED:
                return "Decode image size failed";
        }
    }

private:
    IIFormat m_format = II_FORMAT_UNKNOWN;
    const char *m_ext = "";
    const char *m_fullExt = "";
    const char *m_mimetype = "";
    int64_t m_width = -1;
    int64_t m_height = -1;
    IIErrorCode m_err = II_ERR_OK;

private:
    static std::vector<IIDetector> getDetectors() {
        return {
                ///////////////////////// BMP /////////////////////////
                // https://www.fileformat.info/format/bmp/corion.htm
                IIDetector(
                        II_FORMAT_BMP,
                        "bmp",
                        "bmp",
                        "image/bmp",
                        [](size_t length, IIReadInterface &ri, bool &match, int64_t &width, int64_t &height) {
                            match = length >= 26
                                    && ri.cmp(0, 2, "BM");
                            if (!match) return;

                            // bmp size can be negative, when is negative number, it means flip
                            width = abs(ri.readS32LE(18));
                            height = abs(ri.readS32LE(22));
                        }
                ),

                ///////////////////////// GIF /////////////////////////
                // https://www.fileformat.info/format/gif/corion.htm
                IIDetector(
                        II_FORMAT_GIF,
                        "gif",
                        "gif",
                        "image/gif",
                        [](size_t length, IIReadInterface &ri, bool &match, int64_t &width, int64_t &height) {
                            match = length >= 10
                                    && ri.cmpOneOf(0, 6, {"GIF87a", "GIF89a"});
                            if (!match) return;

                            width = ri.readU16LE(6);
                            height = ri.readU16LE(8);
                        }
                ),

                ///////////////////////// HDR /////////////////////////
                // http://paulbourke.net/dataformats/pic/
                IIDetector(
                        II_FORMAT_HDR,
                        "hdr",
                        "hdr",
                        "image/vnd.radiance",
                        [](size_t length, IIReadInterface &ri, bool &match, int64_t &width, int64_t &height) {
                            match = length >= 6
                                    && ri.cmpOneOf(0, 6, {"#?RGBE", "#?XYZE"});
                            if (!match) return;

                            // TODO Max header size ?
                            auto header = ri.readString(0, length < 256 ? length : 256);
                            std::smatch results;

                            std::regex_search(header, results, std::regex(R"(\s(\-|\+)X\s)"));
                            if (results.empty()) return;
                            off_t xPosBegin = results.position(0) + 4;

                            std::regex_search(header, results, std::regex(R"(\s)"));
                            if (results.empty()) return;
                            off_t xPosEnd = results.position(0);

                            std::regex_search(header, results, std::regex(R"(\s(\-|\+)Y\s)"));
                            if (results.empty()) return;
                            off_t yPosBegin = results.position(0) + 4;

                            std::regex_search(header, results, std::regex(R"(\s)"));
                            if (results.empty()) return;
                            off_t yPosEnd = results.position(0);

                            width = std::stol(header.substr(xPosBegin, xPosEnd));
                            height = std::stol(header.substr(yPosBegin, yPosEnd));
                        }
                ),

                ///////////////////////// JPEG /////////////////////////
                // https://www.fileformat.info/format/jpeg/corion.htm
                IIDetector(
                        II_FORMAT_JPEG,
                        "jpg",
                        "jpeg",
                        "image/jpeg",
                        [](size_t length, IIReadInterface &ri, bool &match, int64_t &width, int64_t &height) {
                            match = length >= 2
                                    && ri.cmp(0, 2, "\xFF\xD8");
                            if (!match) return;

                            int offset = 2;
                            for (; offset + 10 <= length;) {
                                if (ri.cmpOneOf(offset, 2,
                                                {"\xFF\xC0", "\xFF\xC1",
                                                 "\xFF\xC2"})) {    // Found SOF0, SOF1 or SOF2 entry
                                    height = ri.readU16BE(offset + 5);
                                    width = ri.readU16BE(offset + 7);
                                    break;
                                }
                                offset += 2;
                                uint16_t sectionSize = ri.readU16BE(offset);
                                offset += sectionSize;
                            }
                        }
                ),

                ///////////////////////// PNG /////////////////////////
                // https://www.fileformat.info/format/png/corion.htm
                IIDetector(
                        II_FORMAT_PNG,
                        "png",
                        "png",
                        "image/png",
                        [](size_t length, IIReadInterface &ri, bool &match, int64_t &width, int64_t &height) {
                            match = length >= 4
                                    && ri.cmp(0, 4, "\x89PNG");
                            if (!match) return;

                            std::string firstChunkType = ri.readString(12, 4);
                            if (firstChunkType == "IHDR" && length >= 24) {
                                width = ri.readU32BE(16);
                                height = ri.readU32BE(20);
                            } else if (firstChunkType == "CgBI") {
                                if (ri.readString(28, 4) == "IHDR" && length >= 40) {
                                    width = ri.readU32BE(32);
                                    height = ri.readU32BE(36);
                                }
                            }
                        }
                ),

                ///////////////////////// PSD /////////////////////////
                IIDetector(
                        II_FORMAT_PSD,
                        "psd",
                        "psd",
                        "image/psd",
                        [](size_t length, IIReadInterface &ri, bool &match, int64_t &width, int64_t &height) {
                            match = length >= 22
                                    && ri.cmp(0, 6, "8BPS\x00\x01");
                            if (!match) return;

                            height = ri.readU32BE(14);
                            width = ri.readU32BE(18);
                        }
                ),

                ///////////////////////// TIFF /////////////////////////
                // https://www.fileformat.info/format/tiff/corion.htm
                IIDetector(
                        II_FORMAT_TIFF,
                        "tif",
                        "tiff",
                        "image/tiff",
                        [](size_t length, IIReadInterface &ri, bool &match, int64_t &width, int64_t &height) {
                            match = length >= 4
                                    && ri.cmpOneOf(0, 4, {"\x49\x49\x2A\x00", "\x4D\x4D\x00\x2A"});
                            if (!match) return;

                            int needSwap = ri.readU8(0) == 0x4D;

                            uint32_t offset = needSwap ? ri.readU32BE(4) : ri.readU32LE(4);
                            if (length < offset + 2) return;

                            uint16_t numEntry = needSwap ? ri.readU16BE(offset) : ri.readU16LE(offset);

                            offset += 2;
                            for (uint16_t i = 0;
                                 i < numEntry
                                 && length >= offset + 12
                                 && (width == -1 || height == -1);
                                 ++i, offset += 12) {

                                uint16_t tag = needSwap ? ri.readU16BE(offset) : ri.readU16LE(offset);
                                uint16_t type = needSwap ? ri.readU16BE(offset + 2) : ri.readU16LE(offset + 2);

                                if (tag == 256) {           // Found ImageWidth entry
                                    if (type == 3) {
                                        width = needSwap ? ri.readU16BE(offset + 8) : ri.readU16LE(offset + 8);
                                    } else if (type == 4) {
                                        width = needSwap ? ri.readU32BE(offset + 8) : ri.readU32LE(offset + 8);
                                    }
                                } else if (tag == 257) {    // Found ImageHeight entry
                                    if (type == 3) {
                                        height = needSwap ? ri.readU16BE(offset + 8) : ri.readU16LE(offset + 8);
                                    } else if (type == 4) {
                                        height = needSwap ? ri.readU32BE(offset + 8) : ri.readU32LE(offset + 8);
                                    }
                                }
                            }
                        }
                ),

                ///////////////////////// WEBP /////////////////////////
                // https://developers.google.com/speed/webp/docs/riff_container
                IIDetector(
                        II_FORMAT_WEBP,
                        "webp",
                        "webp",
                        "image/webp",
                        [](size_t length, IIReadInterface &ri, bool &match, int64_t &width, int64_t &height) {
                            match = length >= 16
                                    && ri.cmp(0, 4, "RIFF")
                                    && ri.cmp(8, 4, "WEBP");
                            if (!match) return;


                            std::string type = ri.readString(12, 4);
                            // 20
                            if (type == "VP8 " && length >= 30) {
                                width = ri.readU16LE(26) & 0x3FFF;
                                height = ri.readU16LE(28) & 0x3FFF;
                            } else if (type == "VP8L" && length >= 25) {
                                uint32_t n = ri.readU32LE(21);
                                width = (n & 0x3FFF) + 1;
                                height = ((n >> 14) & 0x3FFF) + 1;
                            } else if (type == "VP8X" && length >= 30) {
                                uint8_t extendedHeader = ri.readU8(20);
                                bool validStart = (extendedHeader & 0xc0) == 0;
                                bool validEnd = (extendedHeader & 0x01) == 0;
                                if (validStart && validEnd) {
                                    width = (ri.readU32LE(24) & 0x00FFFFFF) + 1;
                                    height = ((ri.readU32LE(26) & 0xFFFFFF00) >> 8) + 1;
                                } else {
                                    // Invalid
                                }
                            }
                        }
                ),

                ///////////////////////// TGA /////////////////////////
                // https://www.fileformat.info/format/tga/corion.htm
                IIDetector(
                        II_FORMAT_TGA,
                        "tga",
                        "tga",
                        "image/tga",
                        [](size_t length, IIReadInterface &ri, bool &match, int64_t &width, int64_t &height) {

                            // TODO Not rigorous enough, keep it as last detector
                            if (length < 18) {
                                match = false;
                                return;
                            }

                            if (ri.cmp(length - 18, 18, "TRUEVISION-XFILE.\x00")) {
                                match = true;
                                width = ri.readU16LE(12);
                                height = ri.readU16LE(14);
                                return;
                            }

                            uint8_t idLen = ri.readU8(0);
                            if (length < 18 + idLen) {
                                match = false;
                                return;
                            }

                            uint8_t colorMapType = ri.readU8(1);
                            uint8_t imageType = ri.readU8(2);
                            uint16_t firstColorMapEntryIndex = ri.readU16LE(3);
                            uint16_t colorMapLength = ri.readU16LE(5);
                            uint8_t colorMapEntrySize = ri.readU8(7);
                            uint16_t xOrigin = ri.readU16LE(8);
                            uint16_t yOrigin = ri.readU16LE(10);
                            uint16_t w = ri.readU16LE(12);
                            uint16_t h = ri.readU16LE(14);
                            uint8_t pixelDepth = ri.readU8(16);
                            uint8_t flags = ri.readU8(17);

                            if (colorMapType == 0) {            // no color map
                                if (imageType == 0
                                    || imageType == 2
                                    || imageType == 3
                                    || imageType == 10
                                    || imageType == 11
                                    || imageType == 32
                                    || imageType == 33) {
                                    if (firstColorMapEntryIndex == 0
                                        && colorMapLength == 0
                                        && colorMapEntrySize == 0) {
                                        match = true;
                                        width = w;
                                        height = h;
                                        return;
                                    }
                                }
                            } else if (colorMapType == 1) {     // 256 entry palette
                                if (imageType == 1
                                    || imageType == 9) {
                                    match = true;
                                    width = w;
                                    height = h;
                                    return;
                                }
                            }

                            match = false;
                        }
                ),

        };
    }
};


#endif //IMAGEINFO_IMAGEINFO_H
