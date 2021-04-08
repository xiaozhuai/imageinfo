//
// Created by xiaozhuai on 2021/4/1.
// https://github.com/xiaozhuai/imageinfo
//
//
// MIT License
//
// Copyright (c) 2021 xiaozhuai
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
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
#include <unordered_map>
#include <set>
#include <vector>
#include <tuple>
#include <array>
#include <cstdio>

#ifdef ANDROID
#include <android/asset_manager.h>
#endif

template<typename T>
static inline T __ii_swap_endian(T u) {
    static_assert(sizeof(uint8_t) == 1, "sizeof(uint8_t) != 1");
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
    II_FORMAT_CUR,
    II_FORMAT_DDS,
    II_FORMAT_GIF,
    II_FORMAT_HDR,
    II_FORMAT_HEIC,
    II_FORMAT_ICNS,
    II_FORMAT_ICO,
    II_FORMAT_JP2,
    II_FORMAT_JPEG,
    II_FORMAT_JPX,
    II_FORMAT_KTX,
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

#ifdef ANDROID

class IIAndroidAssetFileReader {
public:
    explicit IIAndroidAssetFileReader(AAsset *file) : m_file(file) {}

    inline size_t size() const {
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

#endif

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
            return (size_t) m_file.tellg();
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
            return (size_t) m_file.tellg();
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

class IIBuffer {
public:
    IIBuffer() = default;

    explicit IIBuffer(size_t size) : m_size(size) {
        m_data = std::shared_ptr<uint8_t>(new uint8_t[size], std::default_delete<uint8_t[]>());
    }

    inline const uint8_t *data() const {
        return m_data.get();
    }

    inline uint8_t *data() {
        return m_data.get();
    }

    inline size_t size() const {
        return m_size;
    }

    inline uint8_t &operator[](int offset) {
        return m_data.get()[offset];
    }

    inline uint8_t operator[](int offset) const {
        return m_data.get()[offset];
    }

public:
    inline uint8_t readU8(off_t offset) {
        uint8_t val;
        readInt(offset, val, false);
        return val;
    }

    inline int8_t readS8(off_t offset) {
        int8_t val;
        readInt(offset, val, false);
        return val;
    }

    inline uint16_t readU16LE(off_t offset) {
        uint16_t val;
        readInt(offset, val, false);
        return val;
    }

    inline uint16_t readU16BE(off_t offset) {
        uint16_t val;
        readInt(offset, val, true);
        return val;
    }

    inline int16_t readS16LE(off_t offset) {
        int16_t val;
        readInt(offset, val, false);
        return val;
    }

    inline int16_t readS16BE(off_t offset) {
        int16_t val;
        readInt(offset, val, true);
        return val;
    }

    inline uint32_t readU32LE(off_t offset) {
        uint32_t val;
        readInt(offset, val, false);
        return val;
    }

    inline uint32_t readU32BE(off_t offset) {
        uint32_t val;
        readInt(offset, val, true);
        return val;
    }

    inline int32_t readS32LE(off_t offset) {
        int32_t val;
        readInt(offset, val, false);
        return val;
    }

    inline int32_t readS32BE(off_t offset) {
        int32_t val;
        readInt(offset, val, true);
        return val;
    }

    inline uint64_t readU64LE(off_t offset) {
        uint64_t val;
        readInt(offset, val, false);
        return val;
    }

    inline uint64_t readU64BE(off_t offset) {
        uint64_t val;
        readInt(offset, val, true);
        return val;
    }

    inline int64_t readS64LE(off_t offset) {
        int64_t val;
        readInt(offset, val, false);
        return val;
    }

    inline int64_t readS64BE(off_t offset) {
        int64_t val;
        readInt(offset, val, true);
        return val;
    }

    inline std::string readString(off_t offset, size_t size) {
        return std::string((char *) data() + offset, size);
    }

    inline std::string toString() {
        return std::string((char *) data(), size());
    }

    inline bool cmp(off_t offset, size_t size, const void *mem) {
        return memcmp(data() + offset, mem, size) == 0;
    }

    inline bool cmpOneOf(off_t offset, size_t size, const std::initializer_list<const void *> &mems) {
        for (auto *mem : mems) {
            if (memcmp(data() + offset, mem, size) == 0) return true;
        }
        return false;
    }

private:
    template<typename T>
    inline void readInt(off_t offset, T &val, bool swapEndian = false) {
        val = *((T *) (data() + offset));
        if (swapEndian) {
            val = __ii_swap_endian<T>(val);
        }
    }

private:
    std::shared_ptr<uint8_t> m_data = nullptr;
    size_t m_size = 0;
};

class IIReadInterface {
public:
    IIReadInterface() = delete;

    explicit IIReadInterface(IIReadFunc &r) : read(r) {}

public:
    inline uint8_t readU8(off_t offset) {
        uint8_t val;
        readInt(offset, val, false);
        return val;
    }

    inline int8_t readS8(off_t offset) {
        int8_t val;
        readInt(offset, val, false);
        return val;
    }

    inline uint16_t readU16LE(off_t offset) {
        uint16_t val;
        readInt(offset, val, false);
        return val;
    }

    inline uint16_t readU16BE(off_t offset) {
        uint16_t val;
        readInt(offset, val, true);
        return val;
    }

    inline int16_t readS16LE(off_t offset) {
        int16_t val;
        readInt(offset, val, false);
        return val;
    }

    inline int16_t readS16BE(off_t offset) {
        int16_t val;
        readInt(offset, val, true);
        return val;
    }

    inline uint32_t readU32LE(off_t offset) {
        uint32_t val;
        readInt(offset, val, false);
        return val;
    }

    inline uint32_t readU32BE(off_t offset) {
        uint32_t val;
        readInt(offset, val, true);
        return val;
    }

    inline int32_t readS32LE(off_t offset) {
        int32_t val;
        readInt(offset, val, false);
        return val;
    }

    inline int32_t readS32BE(off_t offset) {
        int32_t val;
        readInt(offset, val, true);
        return val;
    }

    inline uint64_t readU64LE(off_t offset) {
        uint64_t val;
        readInt(offset, val, false);
        return val;
    }

    inline uint64_t readU64BE(off_t offset) {
        uint64_t val;
        readInt(offset, val, true);
        return val;
    }

    inline int64_t readS64LE(off_t offset) {
        int64_t val;
        readInt(offset, val, false);
        return val;
    }

    inline int64_t readS64BE(off_t offset) {
        int64_t val;
        readInt(offset, val, true);
        return val;
    }

    inline IIBuffer readBuffer(off_t offset, size_t size) {
        IIBuffer buffer(size);
        read(buffer.data(), offset, size);
        return buffer;
    }

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
    template<typename T>
    inline void readInt(off_t offset, T &val, bool swapEndian) {
        read(&val, offset, sizeof(T));
        if (swapEndian) {
            val = __ii_swap_endian<T>(val);
        }
    }

private:
    IIReadFunc &read;
};

typedef std::function<void(size_t length, IIReadInterface &ri,
                           bool &match, int64_t &width, int64_t &height,
                           std::vector<std::array<int64_t, 2>> &entrySizes)> IIProcessFunc;

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

static const std::vector<IIDetector> s_ii_detectors = {
        ///////////////////////// BMP /////////////////////////
        // https://www.fileformat.info/format/bmp/corion.htm
        IIDetector(
                II_FORMAT_BMP,
                "bmp",
                "bmp",
                "image/bmp",
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 26) {
                        match = false;
                        return;
                    }
                    auto buffer = ri.readBuffer(0, 26);
                    if (!buffer.cmp(0, 2, "BM")) {
                        match = false;
                        return;
                    }
                    match = true;
                    width = buffer.readS32LE(18);
                    // bmp height can be negative, it means flip Y
                    height = std::abs(buffer.readS32LE(22));
                }
        ),

        ///////////////////////// CUR /////////////////////////
        IIDetector(
                II_FORMAT_CUR,
                "cur",
                "cur",
                "image/cur",
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 6) {
                        match = false;
                        return;
                    }
                    auto buffer = ri.readBuffer(0, 6);
                    // Same with ico, but TYPE == 2
                    if (!buffer.cmp(0, 4, "\x00\x00\x02\x00")) {
                        match = false;
                        return;
                    }
                    uint16_t entryCount = buffer.readU16LE(4);
                    if (entryCount == 0) {
                        match = false;
                        return;
                    }
                    const size_t ENTRY_SIZE = 16;
                    size_t entryTotalSize = entryCount * ENTRY_SIZE;

                    off_t offset = 6;
                    if (length < offset + entryTotalSize) {
                        match = false;
                        return;
                    }
                    buffer = ri.readBuffer(offset, entryTotalSize);
                    offset += entryTotalSize;

                    std::vector<std::array<int64_t, 2>> sizes;

                    for (int i = 0; i < entryCount; ++i) {
                        uint8_t w1 = buffer.readU8(i * ENTRY_SIZE);
                        uint8_t h1 = buffer.readU8(i * ENTRY_SIZE + 1);
                        int64_t w2 = w1 == 0 ? 256 : w1;
                        int64_t h2 = h1 == 0 ? 256 : h1;
                        sizes.push_back({w2, h2});

                        uint32_t bytes = buffer.readS32LE(i * ENTRY_SIZE + 8);
                        offset += bytes;
                    }

                    if (length < (size_t) offset) {
                        match = false;
                        return;
                    }

                    match = true;

                    width = sizes.front()[0];
                    height = sizes.front()[1];

                    sizes.swap(entrySizes);
                }
        ),

        ///////////////////////// DDS /////////////////////////
        IIDetector(
                II_FORMAT_DDS,
                "dds",
                "dds",
                "image/dds",
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 20) {
                        match = false;
                        return;
                    }
                    auto buffer = ri.readBuffer(0, 20);
                    if (!buffer.cmp(0, 4, "DDS ")) {
                        match = false;
                        return;
                    }
                    match = true;
                    height = buffer.readU32LE(12);
                    width = buffer.readU32LE(16);
                }
        ),

        ///////////////////////// GIF /////////////////////////
        // https://www.fileformat.info/format/gif/corion.htm
        IIDetector(
                II_FORMAT_GIF,
                "gif",
                "gif",
                "image/gif",
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 10) {
                        match = false;
                        return;
                    }
                    auto buffer = ri.readBuffer(0, 10);
                    if (!buffer.cmpOneOf(0, 6, {"GIF87a", "GIF89a"})) {
                        match = false;
                        return;
                    }
                    match = true;
                    width = buffer.readU16LE(6);
                    height = buffer.readU16LE(8);
                }
        ),

        ///////////////////////// HDR /////////////////////////
        // http://paulbourke.net/dataformats/pic/
        IIDetector(
                II_FORMAT_HDR,
                "hdr",
                "hdr",
                "image/vnd.radiance",
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 6) {
                        match = false;
                        return;
                    }

                    // TODO Max header size ? Or just read header line by line
                    auto buffer = ri.readBuffer(0, std::min(length, (size_t) 256));
                    if (!buffer.cmpOneOf(0, 6, {"#?RGBE", "#?XYZE"})) {
                        match = false;
                        return;
                    }

                    match = true;

                    auto header = buffer.toString();
                    std::smatch results;

                    std::regex_search(header, results, std::regex(R"(\s(\-|\+)X\s(\d+)\s)"));
                    if (results.size() < 3) return;
                    auto widthStr = results.str(2);

                    std::regex_search(header, results, std::regex(R"(\s(\-|\+)Y\s(\d+)\s)"));
                    if (results.size() < 3) return;
                    auto heightStr = results.str(2);

                    width = std::stol(widthStr);
                    height = std::stol(heightStr);
                }
        ),

        ///////////////////////// HEIC /////////////////////////
        // https://nokiatech.github.io/heif/technical.html
        // https://www.jianshu.com/p/b016d10a087d
        IIDetector(
                II_FORMAT_HEIC,
                "heic",
                "heic",
                "image/heic",
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 36) {
                        match = false;
                        return;
                    }
                    off_t offset = 0;
                    auto buffer = ri.readBuffer(offset, 36);
                    offset += 36;
                    if (!buffer.cmp(4, 4, "ftyp")) {
                        match = false;
                        return;
                    }

                    /**
                     * Major Brand
                     *
                     * HEIF: "mif1", "msf1"
                     * HEIC: "heic", "heix", "hevc", "hevx"
                     *
                     */
                    if (!buffer.cmpOneOf(8, 4, {"mif1", "msf1", "heic", "heix", "hevc", "hevx"})) {
                        match = false;
                        return;
                    }
                    if (!buffer.cmp(28, 4, "meta")) {
                        match = false;
                        return;
                    }

                    match = true;

                    uint32_t metaLength = buffer.readU32BE(24);

                    off_t metaEnd = offset + metaLength;

                    if (length < metaEnd) {
                        match = false;
                        return;
                    }

                    loop_box:

                    /**
                     * find ispe box
                     *
                     * meta
                     *   - ...
                     *   - iprp
                     *       - ...
                     *       - ipco
                     *           - ...
                     *           - ispe
                     */
                    while (offset < metaEnd) {
                        buffer = ri.readBuffer(offset, 8);
                        // std::string boxType = buffer.readString(4, 4);
                        uint32_t boxSize = buffer.readU32BE(0);
                        // std::cout << boxSize << ", " << boxType << "\n";
                        if (buffer.cmpOneOf(4, 4, {"iprp", "ipco"})) {
                            metaEnd = offset + boxSize;
                            offset += 8;
                            goto loop_box;
                        }
                        if (buffer.cmp(4, 4, "ispe")) {
                            buffer = ri.readBuffer(offset + 12, 8);
                            width = buffer.readU32BE(0);
                            height = buffer.readU32BE(4);
                            break;
                        }
                        offset += boxSize;
                    }
                }
        ),

        ///////////////////////// ICNS /////////////////////////
        IIDetector(
                II_FORMAT_ICNS,
                "icns",
                "icns",
                "image/icns",
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 8) {
                        match = false;
                        return;
                    }
                    auto buffer = ri.readBuffer(0, 8);
                    uint32_t fileLength = buffer.readU32BE(4);
                    if (!buffer.cmp(0, 4, "icns") || fileLength != length) {
                        match = false;
                        return;
                    }
                    match = true;

                    const std::unordered_map<std::string, int64_t> TYPE_SIZE_MAP = {
                            {"ICON", 32},
                            {"ICN#", 32},
                            {"icm#", 16},
                            {"icm4", 16},
                            {"icm8", 16},
                            {"ics#", 16},
                            {"ics4", 16},
                            {"ics8", 16},
                            {"is32", 16},
                            {"s8mk", 16},
                            {"icl4", 32},
                            {"icl8", 32},
                            {"il32", 32},
                            {"l8mk", 32},
                            {"ich#", 48},
                            {"ich4", 48},
                            {"ich8", 48},
                            {"ih32", 48},
                            {"h8mk", 48},
                            {"it32", 128},
                            {"t8mk", 128},
                            {"icp4", 16},
                            {"icp5", 32},
                            {"icp6", 64},
                            {"ic07", 128},
                            {"ic08", 256},
                            {"ic09", 512},
                            {"ic10", 1024},
                            {"ic11", 32},
                            {"ic12", 64},
                            {"ic13", 256},
                            {"ic14", 512},
                            {"ic04", 16},
                            {"ic05", 32},
                            {"icsB", 36},
                            {"icsb", 18},
                    };

                    int64_t maxSize = 0;

                    off_t offset = 8;
                    while (offset + 8 <= length) {
                        buffer = ri.readBuffer(offset, 8);
                        auto type = buffer.readString(0, 4);
                        uint32_t entrySize = buffer.readU32BE(4);
                        int64_t s = TYPE_SIZE_MAP.at(type);
                        entrySizes.push_back({s, s});
                        maxSize = std::max(maxSize, s);
                        offset += entrySize;
                    }

                    width = maxSize;
                    height = maxSize;
                }
        ),

        ///////////////////////// ICO /////////////////////////
        IIDetector(
                II_FORMAT_ICO,
                "ico",
                "ico",
                "image/ico",
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 6) {
                        match = false;
                        return;
                    }
                    auto buffer = ri.readBuffer(0, 6);
                    // TYPE == 1
                    if (!buffer.cmp(0, 4, "\x00\x00\x01\x00")) {
                        match = false;
                        return;
                    }
                    uint16_t entryCount = buffer.readU16LE(4);
                    if (entryCount == 0) {
                        match = false;
                        return;
                    }
                    const size_t ENTRY_SIZE = 16;
                    size_t entryTotalSize = entryCount * ENTRY_SIZE;

                    off_t offset = 6;
                    if (length < offset + entryTotalSize) {
                        match = false;
                        return;
                    }
                    buffer = ri.readBuffer(offset, entryTotalSize);
                    offset += entryTotalSize;

                    std::vector<std::array<int64_t, 2>> sizes;

                    for (int i = 0; i < entryCount; ++i) {
                        uint8_t w1 = buffer.readU8(i * ENTRY_SIZE);
                        uint8_t h1 = buffer.readU8(i * ENTRY_SIZE + 1);
                        int64_t w2 = w1 == 0 ? 256 : w1;
                        int64_t h2 = h1 == 0 ? 256 : h1;
                        sizes.push_back({w2, h2});

                        uint32_t bytes = buffer.readS32LE(i * ENTRY_SIZE + 8);
                        offset += bytes;
                    }

                    if (length < (size_t) offset) {
                        match = false;
                        return;
                    }

                    match = true;

                    width = sizes.front()[0];
                    height = sizes.front()[1];

                    sizes.swap(entrySizes);
                }
        ),

        ///////////////////////// JP2 /////////////////////////
        // https://docs.fileformat.com/image/jp2/
        IIDetector(
                II_FORMAT_JP2,
                "jp2",
                "jp2",
                "image/jp2",
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 8) {
                        match = false;
                        return;
                    }
                    auto buffer = ri.readBuffer(0, 8);

                    if (!buffer.cmp(4, 4, "jP  ")) {
                        match = false;
                        return;
                    }

                    uint32_t signatureLength = buffer.readU32BE(0);
                    off_t offset = signatureLength;

                    if (length < offset + 12) {
                        match = false;
                        return;
                    }

                    buffer = ri.readBuffer(offset, 12);
                    // type == "jp2 "
                    if (!buffer.cmp(4, 8, "ftypjp2 ")) {
                        match = false;
                        return;
                    }
                    match = true;

                    uint32_t ftypLength = buffer.readU32BE(0);
                    offset += ftypLength;

                    while (offset + 24 <= length) {
                        buffer = ri.readBuffer(offset, 24);
                        if (buffer.cmp(4, 4, "jp2h")) {
                            if (buffer.cmp(12, 4, "ihdr")) {
                                height = buffer.readU32BE(16);
                                width = buffer.readU32BE(20);
                            }
                            break;
                        }
                        uint32_t boxLength = buffer.readU32BE(0);
                        offset += boxLength;
                    }
                }
        ),

        ///////////////////////// JPEG /////////////////////////
        // https://www.fileformat.info/format/jpeg/corion.htm
        IIDetector(
                II_FORMAT_JPEG,
                "jpg",
                "jpeg",
                "image/jpeg",
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 2) {
                        match = false;
                        return;
                    }
                    if (!ri.cmp(0, 2, "\xFF\xD8")) {
                        match = false;
                        return;
                    }
                    match = true;

                    size_t offset = 2;
                    while (offset + 4 <= length) {
                        auto section = ri.readBuffer(offset, offset + 9 <= length ? 9 : 4);
                        uint16_t sectionSize = section.readU16BE(2);

                        // 0xFFC0 is baseline standard (SOF0)
                        // 0xFFC1 is baseline optimized (SOF1)
                        // 0xFFC2 is progressive (SOF2)
                        if (section.cmpOneOf(0, 2, {"\xFF\xC0", "\xFF\xC1", "\xFF\xC2"})
                            && offset + 9 <= length) {
                            height = section.readU16BE(5);
                            width = section.readU16BE(7);
                            break;
                        }
                        offset += sectionSize + 2;
                    }
                }
        ),

        ///////////////////////// JPX /////////////////////////
        // https://docs.fileformat.com/image/jpx/
        IIDetector(
                II_FORMAT_JPX,
                "jpx",
                "jpx",
                "image/jpx",
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 8) {
                        match = false;
                        return;
                    }
                    auto buffer = ri.readBuffer(0, 8);

                    if (!buffer.cmp(4, 4, "jP  ")) {
                        match = false;
                        return;
                    }

                    uint32_t signatureLength = buffer.readU32BE(0);
                    off_t offset = signatureLength;

                    if (length < offset + 12) {
                        match = false;
                        return;
                    }

                    buffer = ri.readBuffer(offset, 12);
                    // same as jp2, type == "jpx "
                    if (!buffer.cmp(4, 8, "ftypjpx ")) {
                        match = false;
                        return;
                    }
                    match = true;

                    uint32_t ftypLength = buffer.readU32BE(0);
                    offset += ftypLength;

                    while (offset + 24 <= length) {
                        buffer = ri.readBuffer(offset, 24);
                        if (buffer.cmp(4, 4, "jp2h")) {
                            if (buffer.cmp(12, 4, "ihdr")) {
                                height = buffer.readU32BE(16);
                                width = buffer.readU32BE(20);
                            }
                            break;
                        }
                        uint32_t boxLength = buffer.readU32BE(0);
                        offset += boxLength;
                    }
                }
        ),

        ///////////////////////// KTX /////////////////////////
        IIDetector(
                II_FORMAT_KTX,
                "ktx",
                "ktx",
                "image/ktx",
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 44) {
                        match = false;
                        return;
                    }
                    auto buffer = ri.readBuffer(0, 44);
                    if (!buffer.cmp(1, 6, "KTX 11")) {
                        match = false;
                        return;
                    }
                    match = true;
                    width = buffer.readU32LE(36);
                    height = buffer.readU32LE(40);
                }
        ),

        ///////////////////////// PNG /////////////////////////
        // https://www.fileformat.info/format/png/corion.htm
        IIDetector(
                II_FORMAT_PNG,
                "png",
                "png",
                "image/png",
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 4) {
                        match = false;
                        return;
                    }

                    auto buffer = ri.readBuffer(0, std::min(length, (size_t) 40));
                    if (!buffer.cmp(0, 4, "\x89PNG")) {
                        match = false;
                        return;
                    }

                    match = true;

                    std::string firstChunkType = buffer.readString(12, 4);
                    if (firstChunkType == "IHDR" && buffer.size() >= 24) {
                        width = buffer.readU32BE(16);
                        height = buffer.readU32BE(20);
                    } else if (firstChunkType == "CgBI") {
                        if (buffer.readString(28, 4) == "IHDR" && buffer.size() >= 40) {
                            width = buffer.readU32BE(32);
                            height = buffer.readU32BE(36);
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
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 22) {
                        match = false;
                        return;
                    }
                    auto buffer = ri.readBuffer(0, 22);
                    if (!buffer.cmp(0, 6, "8BPS\x00\x01")) {
                        match = false;
                        return;
                    }

                    match = true;

                    height = buffer.readU32BE(14);
                    width = buffer.readU32BE(18);
                }
        ),

        ///////////////////////// TIFF /////////////////////////
        // https://www.fileformat.info/format/tiff/corion.htm
        IIDetector(
                II_FORMAT_TIFF,
                "tif",
                "tiff",
                "image/tiff",
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 8) {
                        match = false;
                        return;
                    }
                    auto header = ri.readBuffer(0, 8);
                    if (!header.cmpOneOf(0, 4, {"\x49\x49\x2A\x00", "\x4D\x4D\x00\x2A"})) {
                        match = false;
                        return;
                    }
                    match = true;
                    bool needSwap = header[0] == 0x4D;

                    uint32_t offset = needSwap ? header.readU32BE(4) : header.readU32LE(4);
                    if (length < offset + 2) return;

                    uint16_t numEntry = needSwap ? ri.readU16BE(offset) : ri.readU16LE(offset);

                    offset += 2;

                    for (uint16_t i = 0;
                         i < numEntry
                         && length >= offset + 12
                         && (width == -1 || height == -1);
                         ++i, offset += 12) {

                        auto buffer = ri.readBuffer(offset, 12);

                        uint16_t tag = needSwap ? buffer.readU16BE(0) : buffer.readU16LE(0);
                        uint16_t type = needSwap ? buffer.readU16BE(2) : buffer.readU16LE(2);

                        if (tag == 256) {           // Found ImageWidth entry
                            if (type == 3) {
                                width = needSwap ? buffer.readU16BE(8) : buffer.readU16LE(8);
                            } else if (type == 4) {
                                width = needSwap ? buffer.readU32BE(8) : buffer.readU32LE(8);
                            }
                        } else if (tag == 257) {    // Found ImageHeight entry
                            if (type == 3) {
                                height = needSwap ? buffer.readU16BE(8) : buffer.readU16LE(8);
                            } else if (type == 4) {
                                height = needSwap ? buffer.readU32BE(8) : buffer.readU32LE(8);
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
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 16) {
                        match = false;
                        return;
                    }
                    auto buffer = ri.readBuffer(0, std::min(length, (size_t) 30));
                    if (!buffer.cmp(0, 4, "RIFF") || !buffer.cmp(8, 4, "WEBP")) {
                        match = false;
                        return;
                    }

                    match = true;

                    std::string type = buffer.readString(12, 4);
                    if (type == "VP8 " && buffer.size() >= 30) {
                        width = buffer.readU16LE(26) & 0x3FFF;
                        height = buffer.readU16LE(28) & 0x3FFF;
                    } else if (type == "VP8L" && buffer.size() >= 25) {
                        uint32_t n = buffer.readU32LE(21);
                        width = (n & 0x3FFF) + 1;
                        height = ((n >> 14) & 0x3FFF) + 1;
                    } else if (type == "VP8X" && buffer.size() >= 30) {
                        uint8_t extendedHeader = buffer.readU8(20);
                        bool validStart = (extendedHeader & 0xc0) == 0;
                        bool validEnd = (extendedHeader & 0x01) == 0;
                        if (validStart && validEnd) {
                            width = (buffer.readU32LE(24) & 0x00FFFFFF) + 1;
                            height = ((buffer.readU32LE(26) & 0xFFFFFF00) >> 8) + 1;
                        } else {
                            // Invalid
                        }
                    }
                }
        ),

        ///////////////////////// TGA /////////////////////////
        // TODO Not rigorous enough, keep it as last detector
        // https://www.fileformat.info/format/tga/corion.htm
        IIDetector(
                II_FORMAT_TGA,
                "tga",
                "tga",
                "image/tga",
                [](size_t length, IIReadInterface &ri,
                   bool &match, int64_t &width, int64_t &height,
                   std::vector<std::array<int64_t, 2>> &entrySizes) {
                    if (length < 18) {
                        match = false;
                        return;
                    }

                    auto buffer = ri.readBuffer(0, 18);

                    if (ri.cmp(length - 18, 18, "TRUEVISION-XFILE.\x00")) {
                        match = true;
                        width = buffer.readU16LE(12);
                        height = buffer.readU16LE(14);
                        return;
                    }

                    uint8_t idLen = buffer.readU8(0);
                    if (length < (size_t) idLen + 18) {
                        match = false;
                        return;
                    }

                    uint8_t colorMapType = buffer.readU8(1);
                    uint8_t imageType = buffer.readU8(2);
                    uint16_t firstColorMapEntryIndex = buffer.readU16LE(3);
                    uint16_t colorMapLength = buffer.readU16LE(5);
                    uint8_t colorMapEntrySize = buffer.readU8(7);
                    uint16_t xOrigin = buffer.readU16LE(8);
                    uint16_t yOrigin = buffer.readU16LE(10);
                    uint16_t w = buffer.readU16LE(12);
                    uint16_t h = buffer.readU16LE(14);
                    uint8_t pixelDepth = buffer.readU8(16);
                    uint8_t flags = buffer.readU8(17);

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

template<typename InputType, typename ReaderType>
class ImageInfo {
public:
    ImageInfo() = delete;

    explicit ImageInfo(InputType file, IIFormat likelyFormat = II_FORMAT_UNKNOWN, bool mustBe = false) {
        ReaderType fileReader(file);
        IIReadFunc read = [&](void *buf, off_t offset, size_t size) { fileReader.read(buf, offset, size); };
        IIReadInterface ri(read);
        size_t length = fileReader.size();

        if (likelyFormat != II_FORMAT_UNKNOWN) {
            for (const auto &detector : s_ii_detectors) {
                if (detector.format == likelyFormat) {
                    if (tryDetector(detector, length, ri)) return;
                    break;
                }
            }
            if (mustBe) {
                m_err = II_ERR_UNRECOGNIZED_FORMAT;
                return;
            }
        }

        for (const auto &detector : s_ii_detectors) {
            if (detector.format == likelyFormat) continue;
            if (tryDetector(detector, length, ri)) return;
        }

        m_err = II_ERR_UNRECOGNIZED_FORMAT;
    }

    bool tryDetector(const IIDetector &detector, size_t length, IIReadInterface &ri) {
        bool match = false;
        detector.process(length, ri, match, m_width, m_height, m_entrySizes);
        if (match) {
            m_format = detector.format;
            m_ext = detector.ext;
            m_fullExt = detector.fullExt;
            m_mimetype = detector.mimetype;
            m_err = m_width != -1 && m_height != -1
                    ? II_ERR_OK
                    : II_ERR_DECODE_SIZE_FAILED;
        }
        return match;
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

    inline const std::vector<std::array<int64_t, 2>> &getEntrySizes() const {
        if (m_entrySizes.empty()) {
            return {getSize()};
        }
        return m_entrySizes;
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
    std::vector<std::array<int64_t, 2>> m_entrySizes;
};


#endif //IMAGEINFO_IMAGEINFO_H
