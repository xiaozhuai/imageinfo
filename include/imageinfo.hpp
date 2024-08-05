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

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <limits>
#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#ifdef ANDROID
#include <android/asset_manager.h>
#endif

#ifndef II_HEADER_CACHE_SIZE
#define II_HEADER_CACHE_SIZE (1024)
#endif

// #define II_DISABLE_HEADER_CACHE

static_assert(sizeof(uint8_t) == 1, "sizeof(uint8_t) != 1");
static_assert(sizeof(int8_t) == 1, "sizeof(int8_t) != 1");
static_assert(sizeof(uint16_t) == 2, "sizeof(uint16_t) != 2");
static_assert(sizeof(int16_t) == 2, "sizeof(int16_t) != 2");
static_assert(sizeof(uint32_t) == 4, "sizeof(uint32_t) != 4");
static_assert(sizeof(int32_t) == 4, "sizeof(int32_t) != 4");
static_assert(sizeof(uint64_t) == 8, "sizeof(uint64_t) != 8");
static_assert(sizeof(int64_t) == 8, "sizeof(int64_t) != 8");

#ifdef __clang__
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#endif

namespace imageinfo {

enum Format {
    kFormatUnknown = 0,
    kFormatAvif,
    kFormatBmp,
    kFormatCur,
    kFormatDds,
    kFormatGif,
    kFormatHdr,
    kFormatHeic,
    kFormatIcns,
    kFormatIco,
    kFormatJ2k,
    kFormatJp2,
    kFormatJph,
    kFormatJpx,
    kFormatJpeg,
    kFormatKtx,
    kFormatPng,
    kFormatPsd,
    kFormatQoi,
    kFormatTiff,
    kFormatWebp,
    kFormatTga,
    //
    FORMAT_END,
    FORMAT_COUNT = FORMAT_END - 1,
};

enum Error {
    kNoError = 0,
    kUnrecognizedFormat,
};

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

class FilePathReader {
public:
    explicit FilePathReader(const std::string &path) : file_(path, std::ios::in | std::ios::binary) {}

    ~FilePathReader() {
        if (file_.is_open()) {
            file_.close();
        }
    }

    inline size_t size() {
        if (file_.is_open()) {
            file_.seekg(0, std::ios::end);
            return (size_t)file_.tellg();
        } else {
            return 0;
        }
    }

    inline void read(void *buf, off_t offset, size_t size) {
        file_.seekg(offset, std::ios::beg);
        file_.read((char *)buf, (std::streamsize)size);
    }

private:
    std::ifstream file_;
};

class FileStreamReader {
public:
    explicit FileStreamReader(std::ifstream &file) : file_(file) {}

    inline size_t size() {
        if (file_.is_open()) {
            file_.seekg(0, std::ios::end);
            return (size_t)file_.tellg();
        } else {
            return 0;
        }
    }

    inline void read(void *buf, off_t offset, size_t size) {
        file_.seekg(offset, std::ios::beg);
        file_.read((char *)buf, (std::streamsize)size);
    }

private:
    std::ifstream &file_;
};

#ifdef ANDROID

class AndroidAssetFileReader {
public:
    explicit AndroidAssetFileReader(AAsset *file) : file_(file) {}

    inline size_t size() const {
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

#endif

struct RawData {
    RawData(const void *d, size_t s) : data(d), length(s) {}

    const void *data = nullptr;
    size_t length = 0;
};

class RawDataReader {
public:
    explicit RawDataReader(RawData data) : data_(data) {}

    inline size_t size() const { return data_.length; }

    inline void read(void *buf, off_t offset, size_t size) const { memcpy(buf, ((char *)data_.data) + offset, size); }

private:
    RawData data_;
};

class Buffer {
public:
    Buffer() = default;

    explicit Buffer(size_t size) { alloc(size); }

    inline void alloc(size_t size) {
        size_ = size;
        data_ = std::shared_ptr<uint8_t>(new uint8_t[size], std::default_delete<uint8_t[]>());
    }

    inline const uint8_t *data() const { return data_.get(); }

    inline uint8_t *data() { return data_.get(); }

    inline size_t size() const { return size_; }

    inline uint8_t &operator[](int offset) { return data_.get()[offset]; }

    inline uint8_t operator[](int offset) const { return data_.get()[offset]; }

public:
    inline uint8_t read_u8(off_t offset) { return read_int<uint8_t>(offset, false); }

    inline int8_t read_s8(off_t offset) { return read_int<int8_t>(offset, false); }

    inline uint16_t read_u16_le(off_t offset) { return read_int<uint16_t>(offset, false); }

    inline uint16_t read_u16_be(off_t offset) { return read_int<uint16_t>(offset, true); }

    inline int16_t read_s16_le(off_t offset) { return read_int<int16_t>(offset, false); }

    inline int16_t read_s16_be(off_t offset) { return read_int<int16_t>(offset, true); }

    inline uint32_t read_u32_le(off_t offset) { return read_int<uint32_t>(offset, false); }

    inline uint32_t read_u32_be(off_t offset) { return read_int<uint32_t>(offset, true); }

    inline int32_t read_s32_le(off_t offset) { return read_int<int32_t>(offset, false); }

    inline int32_t read_s32_be(off_t offset) { return read_int<int32_t>(offset, true); }

    inline uint64_t read_u64_le(off_t offset) { return read_int<uint64_t>(offset, false); }

    inline uint64_t read_u64_be(off_t offset) { return read_int<uint64_t>(offset, true); }

    inline int64_t read_s64_le(off_t offset) { return read_int<int64_t>(offset, false); }

    inline int64_t read_s64_be(off_t offset) { return read_int<int64_t>(offset, true); }

    template <typename T>
    inline T read_int(off_t offset, bool swap_endian = false) {
        T val = *((T *)(data() + offset));
        return swap_endian ? swap_e<T>(val) : val;
    }

    inline std::string read_string(off_t offset, size_t size) { return std::string((char *)data() + offset, size); }

    inline std::string to_string() { return std::string((char *)data(), size()); }

    inline bool cmp(off_t offset, size_t size, const void *buf) { return memcmp(data() + offset, buf, size) == 0; }

    inline bool cmp_any_of(off_t offset, size_t size, const std::initializer_list<const void *> &bufs) {
        return std::any_of(bufs.begin(), bufs.end(),
                           [this, offset, size](const void *buf) { return memcmp(data() + offset, buf, size) == 0; });
    }

private:
    template <typename T>
    static inline T swap_e(T u) {
        union {
            T u;
            uint8_t u8[sizeof(T)];
        } src{}, dst{};
        src.u = u;
        for (size_t k = 0; k < sizeof(T); k++) {
            dst.u8[k] = src.u8[sizeof(T) - k - 1];
        }
        return dst.u;
    }

private:
    std::shared_ptr<uint8_t> data_ = nullptr;
    size_t size_ = 0;
};

using ReadFunc = std::function<void(void *buf, off_t offset, size_t size)>;

class ReadInterface {
public:
    ReadInterface() = delete;

    ReadInterface(ReadFunc &read_func, size_t length) : read_func_(read_func), length_(length) {
#ifndef II_DISABLE_HEADER_CACHE
        header_cache_.alloc((std::min)((size_t)II_HEADER_CACHE_SIZE, length));
        read(header_cache_.data(), 0, header_cache_.size());
#endif
    }

    inline Buffer read_buffer(off_t offset, size_t size) {
        assert(offset >= 0);
        assert(offset + size <= length_);
        Buffer buffer(size);
#ifndef II_DISABLE_HEADER_CACHE
        if (offset + size <= header_cache_.size()) {
            memcpy(buffer.data(), header_cache_.data() + offset, size);
        } else if (offset < header_cache_.size() && header_cache_.size() - offset >= (II_HEADER_CACHE_SIZE / 4)) {
            size_t head = header_cache_.size() - offset;
            memcpy(buffer.data(), header_cache_.data() + offset, head);
            read(buffer.data() + head, offset + (off_t)head, size - head);
        } else {
            read(buffer.data(), offset, size);
        }
#else
        read(buffer.data(), offset, size);
#endif
        return buffer;
    }

    inline size_t length() const { return length_; }

private:
    inline void read(void *buf, off_t offset, size_t size) { read_func_(buf, offset, size); }

private:
    ReadFunc &read_func_;
    size_t length_ = 0;
#ifndef II_DISABLE_HEADER_CACHE
    Buffer header_cache_;
#endif
};

class ImageSize {
public:
    ImageSize() = default;

    ImageSize(int64_t width, int64_t height) : width(width), height(height) {}

    inline bool operator==(const ImageSize &rhs) const { return width == rhs.width && height == rhs.height; }

    inline int64_t operator[](int index) const {
        assert(index >= 0 && index < 2);
        return index == 0 ? width : height;
    }

    int64_t width = -1;
    int64_t height = -1;
};

using EntrySizes = std::vector<ImageSize>;

class ImageInfo {
public:
    ImageInfo() = default;
    explicit ImageInfo(Error error) : error_(error) {}
    ImageInfo(Format format, const char *ext, const char *full_ext, const char *mimetype)
        : format_(format), ext_(ext), full_ext_(full_ext), mimetype_(mimetype) {}

public:
    inline void set_size(const ImageSize &size) { size_ = size; }

    inline void set_size(int64_t width, int64_t height) { size_ = ImageSize(width, height); }

    inline void set_entry_sizes(const EntrySizes &entry_sizes) { entry_sizes_ = entry_sizes; }

    inline void add_entry_size(const ImageSize &size) { entry_sizes_.emplace_back(size); }

    inline void add_entry_size(int64_t width, int64_t height) { entry_sizes_.emplace_back(width, height); }

public:
    inline explicit operator bool() const { return error_ == kNoError; }

    inline bool ok() const { return error_ == kNoError; }

    inline Error error() const { return error_; }

    inline const char *error_msg() const {
        switch (error_) {
            case kNoError:
                return "No error";
            case kUnrecognizedFormat:
                return "Unrecognized format";
            default:
                return "Unknown error";
        }
    }

    inline Format format() const { return format_; }

    inline const char *ext() const { return ext_; }

    inline const char *full_ext() const { return full_ext_; }

    inline const char *mimetype() const { return mimetype_; }

    inline const ImageSize &size() const { return size_; }

    inline const EntrySizes &entry_sizes() const { return entry_sizes_; }

private:
    Format format_ = kFormatUnknown;
    const char *ext_ = "";
    const char *full_ext_ = "";
    const char *mimetype_ = "";
    ImageSize size_;
    EntrySizes entry_sizes_;
    Error error_ = kNoError;
};

inline bool is_numeric(const std::string &str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// https://nokiatech.github.io/heif/technical.html
// https://www.jianshu.com/p/b016d10a087d
// https://github.com/ksvc/MediaParser
inline bool try_avif_heic(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 4) {
        return false;
    }
    auto buffer = ri.read_buffer(0, 4);
    uint32_t ftyp_box_length = buffer.read_u32_be(0);
    if (uint64_t(length) < uint64_t(ftyp_box_length) + 12) {
        return false;
    }
    buffer = ri.read_buffer(0, ftyp_box_length + 12);
    if (!buffer.cmp(4, 4, "ftyp")) {
        return false;
    }

    /**
     * Major Brand
     *
     * AVIF: "avif", "avis"
     * HEIF: "mif1", "msf1"
     * HEIC: "heic", "heix", "hevc", "hevx"
     *
     */
    if (!buffer.cmp_any_of(8, 4, {"avif", "avis", "mif1", "msf1", "heic", "heix", "hevc", "hevx"})) {
        return false;
    }

    if (ftyp_box_length < 16 || (ftyp_box_length - 16) % 4 != 0) {
        return false;
    }
    uint32_t compatible_brand_size = (ftyp_box_length - 16) / 4;
    std::unordered_set<std::string> compatible_brands;
    for (uint32_t i = 0; i < compatible_brand_size; ++i) {
        compatible_brands.insert(buffer.read_string(16 + i * 4, 4));
    }

    Format format;
    const char *ext;
    const char *full_ext;
    const char *mimetype;
    if (compatible_brands.find("avif") != compatible_brands.end() || buffer.cmp(8, 4, "avif")) {
        format = kFormatAvif;
        ext = "avif";
        full_ext = "avif";
        mimetype = "image/avif";
    } else if (compatible_brands.find("heic") != compatible_brands.end() || buffer.cmp(8, 4, "heic")) {
        format = kFormatHeic;
        ext = "heic";
        full_ext = "heic";
        mimetype = "image/heic";
    } else {
        return false;
    }

    if (!buffer.cmp(ftyp_box_length + 4, 4, "meta")) {
        return false;
    }

    uint32_t meta_length = buffer.read_u32_be(ftyp_box_length);

    if (uint64_t(length) < uint64_t(ftyp_box_length) + 12 + uint64_t(meta_length)) {
        return false;
    }

    buffer = ri.read_buffer(ftyp_box_length + 12, meta_length);

    off_t offset = 0;
    off_t end = meta_length;

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
    uint16_t pitm_id = 0;
    std::unordered_map<uint16_t, std::unordered_set<uint8_t>> ipma_map;
    off_t ipco_start = 0;
    off_t ipco_end = 0;
    uint8_t ipco_child_index = 1;
    std::unordered_map<uint8_t, ImageSize> ispe_map;
    std::unordered_map<uint8_t, uint8_t> irot_map;
    while (offset < end) {
        if (offset + 8 > end) {
            break;
        }
        uint32_t box_size = buffer.read_u32_be(offset);
        if (box_size < 8 || uint64_t(offset) + uint64_t(box_size) > uint64_t(end)) {
            break;
        }
        if (buffer.cmp(offset + 4, 4, "pitm")) {
            if (box_size < 14) {
                return false;
            }
            pitm_id = buffer.read_u16_be(offset + 12);
            offset += box_size;
        } else if (buffer.cmp(offset + 4, 4, "ipma")) {
            if (box_size < 16) {
                return false;
            }
            uint16_t entry_count = buffer.read_u16_be(offset + 14);
            off_t t = offset + 16;
            for (uint16_t i = 0; i < entry_count; ++i) {
                if (box_size < 18) {
                    return false;
                }
                uint16_t item_id = buffer.read_u16_be(t);
                t += 2;
                if (box_size < 19) {
                    return false;
                }
                uint8_t index_count = buffer.read_u8(t);
                t += 1;
                if (box_size < 19 + index_count) {
                    return false;
                }
                std::unordered_set<uint8_t> indices;
                for (uint8_t j = 0; j < index_count; ++j, ++t) {
                    auto index = buffer.read_u8(t) & 0x0F;
                    indices.insert(index);
                }
                ipma_map[item_id] = indices;
            }
            offset += box_size;
        } else if (buffer.cmp(offset + 4, 4, "iprp")) {
            offset += 8;
        } else if (buffer.cmp(offset + 4, 4, "ipco")) {
            ipco_start = offset;
            ipco_end = offset + box_size;
            offset += 8;
        } else if (buffer.cmp(offset + 4, 4, "ispe")) {
            if (box_size < 20) {
                return false;
            }
            auto width = buffer.read_u32_be(offset + 12);
            auto height = buffer.read_u32_be(offset + 16);
            ispe_map[ipco_child_index] = ImageSize(width, height);
            ipco_child_index++;
            offset += box_size;
        } else if (buffer.cmp(offset + 4, 4, "irot")) {
            if (box_size < 9) {
                return false;
            }
            uint8_t irot = buffer.read_u8(offset + 8);
            irot_map[ipco_child_index] = irot;
            ipco_child_index++;
            offset += box_size;
        } else {
            if (offset > ipco_start && offset < ipco_end) {
                ipco_child_index++;
            }
            offset += box_size;
        }
    }

    auto ipma_it = ipma_map.find(pitm_id);
    if (ipma_it == ipma_map.end()) {
        return false;
    }
    auto &indices = ipma_it->second;
    uint8_t irot = 0;
    for (const auto &pair : irot_map) {
        auto index = pair.first;
        if (indices.find(index) != indices.end()) {
            irot = pair.second;
            break;
        }
    }
    for (const auto &pair : ispe_map) {
        auto index = pair.first;
        if (indices.find(index) != indices.end()) {
            auto size = pair.second;
            if (irot == 1 || irot == 3 || irot == 6 || irot == 7) {
                std::swap(size.width, size.height);
            }
            info = ImageInfo(format, ext, full_ext, mimetype);
            info.set_size(size);
            return true;
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// https://www.fileformat.info/format/bmp/corion.htm
inline bool try_bmp(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 26) {
        return false;
    }
    auto buffer = ri.read_buffer(0, 26);
    if (!buffer.cmp(0, 2, "BM")) {
        return false;
    }

    info = ImageInfo(kFormatBmp, "bmp", "bmp", "image/bmp");
    // bmp height can be negative, it means flip Y
    info.set_size(                        //
        buffer.read_s32_le(18),           //
        std::abs(buffer.read_s32_le(22))  //
    );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool try_cur_ico(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 6) {
        return false;
    }
    auto buffer = ri.read_buffer(0, 6);

    Format format;
    const char *ext;
    const char *full_ext;
    const char *mimetype;
    if (buffer.cmp(0, 4, "\x00\x00\x01\x00")) {
        format = kFormatIco;
        ext = "ico";
        full_ext = "ico";
        mimetype = "image/ico";
    } else if (buffer.cmp(0, 4, "\x00\x00\x02\x00")) {
        format = kFormatCur;
        ext = "cur";
        full_ext = "cur";
        mimetype = "image/cur";
    } else {
        return false;
    }

    uint16_t entry_count = buffer.read_u16_le(4);
    if (entry_count == 0) {
        return false;
    }
    const int entry_size = 16;
    off_t entry_total_size = entry_count * entry_size;

    off_t offset = 6;
    if (length < offset + entry_total_size) {
        return false;
    }
    buffer = ri.read_buffer(offset, entry_total_size);
    offset += entry_total_size;

    EntrySizes sizes;

    for (int i = 0; i < entry_count; ++i) {
        uint8_t w1 = buffer.read_u8(i * entry_size);
        uint8_t h1 = buffer.read_u8(i * entry_size + 1);
        int64_t w2 = w1 == 0 ? 256 : w1;
        int64_t h2 = h1 == 0 ? 256 : h1;
        sizes.emplace_back(w2, h2);

        uint32_t bytes = buffer.read_s32_le(i * entry_size + 8);
        offset += bytes;
    }

    if (length < (size_t)offset) {
        return false;
    }

    info = ImageInfo(format, ext, full_ext, mimetype);
    info.set_entry_sizes(sizes);
    info.set_size(sizes.front());
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool try_dds(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 20) {
        return false;
    }
    auto buffer = ri.read_buffer(0, 20);
    if (!buffer.cmp(0, 4, "DDS ")) {
        return false;
    }

    info = ImageInfo(kFormatDds, "dds", "dds", "image/dds");
    info.set_size(               //
        buffer.read_u32_le(16),  //
        buffer.read_u32_le(12)   //
    );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// https://www.fileformat.info/format/gif/corion.htm
inline bool try_gif(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 10) {
        return false;
    }
    auto buffer = ri.read_buffer(0, 10);
    if (!buffer.cmp_any_of(0, 6, {"GIF87a", "GIF89a"})) {
        return false;
    }

    info = ImageInfo(kFormatGif, "gif", "gif", "image/gif");
    info.set_size(              //
        buffer.read_u16_le(6),  //
        buffer.read_u16_le(8)   //
    );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// http://paulbourke.net/dataformats/pic/
inline bool try_hdr(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 6) {
        return false;
    }
    auto buffer = ri.read_buffer(0, 6);
    if (!buffer.cmp_any_of(0, 6, {"#?RGBE", "#?XYZE"})) {
        if (length < 10) {
            return false;
        }
        buffer = ri.read_buffer(0, 10);
        if (!buffer.cmp(0, 10, "#?RADIANCE")) {
            return false;
        }
    }

    const size_t piece = 64;
    std::string header;
    size_t resolution_start = 0;
    std::string resolution;
    off_t offset = 0;
    while (offset < length) {
        buffer = ri.read_buffer(offset, std::min<size_t>(length - offset, piece));
        size_t start_pos = buffer.size() - 1;
        offset += (off_t)buffer.size();
        header += buffer.to_string();
        if (resolution_start == 0) {
            auto pos = header.find("\n\n", start_pos);
            if (pos == std::string::npos) {
                continue;
            }
            resolution_start = pos + 2;
        }
        auto pos = header.find('\n', resolution_start);
        if (pos != std::string::npos) {
            resolution = header.substr(resolution_start, pos - resolution_start);
            break;
        }
    }
    if (resolution.empty()) {
        return false;
    }
    auto p0 = resolution.find(' ');
    if (p0 == std::string::npos) {
        return false;
    }
    auto p1 = resolution.find(' ', p0 + 1);
    if (p1 == std::string::npos) {
        return false;
    }
    auto p2 = resolution.find(' ', p1 + 1);
    if (p2 == std::string::npos) {
        return false;
    }
    auto y_str = resolution.substr(p0 + 1, p1 - p0 - 1);
    auto x_str = resolution.substr(p2 + 1);
    if (!is_numeric(x_str) || !is_numeric(y_str)) {
        return false;
    }
    auto safe_stou32 = [](const std::string &str, uint32_t &result) {
        if (str.empty()) {
            return false;
        }
        result = 0;
        uint32_t limit = std::numeric_limits<uint32_t>::max() / 10;
        uint32_t limit_mod = std::numeric_limits<uint32_t>::max() % 10;
        for (char c : str) {
            if (c < '0' || c > '9') {
                return false;
            }
            int digit = c - '0';
            if (result > limit || (result == limit && digit > limit_mod)) {
                return false;
            }
            result = result * 10 + digit;
        }
        return true;
    };
    uint32_t x, y;
    if (!safe_stou32(x_str, x) || !safe_stou32(y_str, y)) {
        return false;
    }
    if (x == 0 || y == 0) {
        return false;
    }
    info = ImageInfo(kFormatHdr, "hdr", "hdr", "image/vnd.radiance");
    info.set_size(x, y);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool try_icns(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 8) {
        return false;
    }
    auto buffer = ri.read_buffer(0, 8);
    uint32_t file_length = buffer.read_u32_be(4);
    if (!buffer.cmp(0, 4, "icns") || file_length != length) {
        return false;
    }

    static const std::unordered_map<std::string, int64_t> size_map = {
        {"ICON",   32},
        {"ICN#",   32},
        {"icm#",   16},
        {"icm4",   16},
        {"icm8",   16},
        {"ics#",   16},
        {"ics4",   16},
        {"ics8",   16},
        {"is32",   16},
        {"s8mk",   16},
        {"icl4",   32},
        {"icl8",   32},
        {"il32",   32},
        {"l8mk",   32},
        {"ich#",   48},
        {"ich4",   48},
        {"ich8",   48},
        {"ih32",   48},
        {"h8mk",   48},
        {"it32",  128},
        {"t8mk",  128},
        {"icp4",   16},
        {"icp5",   32},
        {"icp6",   64},
        {"ic07",  128},
        {"ic08",  256},
        {"ic09",  512},
        {"ic10", 1024},
        {"ic11",   32},
        {"ic12",   64},
        {"ic13",  256},
        {"ic14",  512},
        {"ic04",   16},
        {"ic05",   32},
        {"icsB",   36},
        {"icsb",   18},
    };

    int64_t max_size = 0;
    EntrySizes entry_sizes;

    off_t offset = 8;
    while (offset + 8 <= length) {
        buffer = ri.read_buffer(offset, 8);
        auto type = buffer.read_string(0, 4);
        uint32_t entry_size = buffer.read_u32_be(4);
        auto it = size_map.find(type);
        if (it != size_map.end()) {
            int64_t s = it->second;
            entry_sizes.emplace_back(s, s);
            max_size = std::max(max_size, s);
        } else {
            return false;
        }
        offset += entry_size;
    }

    info = ImageInfo(kFormatIcns, "icns", "icns", "image/icns");
    info.set_size(max_size, max_size);
    info.set_entry_sizes(entry_sizes);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// https://docs.fileformat.com/image/jp2/
// https://docs.fileformat.com/image/jpx/
inline bool try_jpeg2000_code_stream(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 16) {
        return false;
    }
    auto buffer = ri.read_buffer(0, 16);

    // SOC and SIZ
    if (buffer.cmp(0, 2, "\xFF\x4F") && buffer.cmp(2, 2, "\xFF\x51")) {
        uint16_t siz_length = buffer.read_u16_be(4);
        if (length < siz_length + 4) {
            return false;
        }
        info = ImageInfo(kFormatJ2k, "j2k", "j2k", "image/j2k");
        info.set_size(              //
            buffer.read_u32_be(8),  //
            buffer.read_u32_be(12)  //
        );
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// https://docs.fileformat.com/image/jp2/
// https://docs.fileformat.com/image/jpx/
inline bool try_jpeg2000(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 8) {
        return false;
    }
    auto buffer = ri.read_buffer(0, 8);

    if (!buffer.cmp(4, 4, "jP  ")) {
        return false;
    }

    uint32_t signature_length = buffer.read_u32_be(0);
    off_t offset = signature_length;

    if (length < offset + 12) {
        return false;
    }

    buffer = ri.read_buffer(offset, 12);
    if (!buffer.cmp(4, 4, "ftyp")) {
        return false;
    }

    Format format;
    const char *ext;
    const char *full_ext;
    const char *mimetype;
    if (buffer.cmp(8, 4, "jp2 ")) {
        format = kFormatJp2;
        ext = "jp2";
        full_ext = "jp2";
        mimetype = "image/jp2";
    } else if (buffer.cmp(8, 4, "jph ")) {
        format = kFormatJph;
        ext = "jph";
        full_ext = "jph";
        mimetype = "image/jph";
    } else if (buffer.cmp(8, 4, "jpx ")) {
        format = kFormatJpx;
        ext = "jpx";
        full_ext = "jpx";
        mimetype = "image/jpx";
    } else {
        return false;
    }

    uint32_t ftyp_length = buffer.read_u32_be(0);
    offset += ftyp_length;

    while (offset + 24 <= length) {
        buffer = ri.read_buffer(offset, 24);
        if (buffer.cmp(4, 4, "jp2h")) {
            if (buffer.cmp(12, 4, "ihdr")) {
                info = ImageInfo(format, ext, full_ext, mimetype);
                info.set_size(               //
                    buffer.read_u32_be(20),  //
                    buffer.read_u32_be(16)   //
                );
                return true;
            } else {
                return false;
            }
        }
        uint32_t box_length = buffer.read_u32_be(0);
        offset += box_length;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// https://www.fileformat.info/format/jpeg/corion.htm
inline bool try_jpg(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 2) {
        return false;
    }
    auto buffer = ri.read_buffer(0, 2);
    if (!buffer.cmp(0, 2, "\xFF\xD8")) {
        return false;
    }

    uint16_t orientation = 1;
    off_t offset = 2;
    while (offset + 9 <= length) {
        buffer = ri.read_buffer(offset, 9);
        uint16_t section_size = buffer.read_u16_be(2);
        if (!buffer.cmp(0, 1, "\xFF")) {
            // skip garbage bytes
            offset += 1;
            continue;
        }

        // 0xFFE1 is application 1 (APP1)
        if (buffer.cmp(0, 2, "\xFF\xE1")) {
            if (offset + section_size + 2 > length) {
                return false;
            }
            buffer = ri.read_buffer(offset, section_size + 2);
            if (buffer.cmp(4, 5, "Exif\0")) {
                bool big_endian = !buffer.cmp(10, 1, "I");
                auto first_ifd_offset = buffer.read_int<uint32_t>(14, big_endian);
                if (first_ifd_offset < 8 || uint64_t(first_ifd_offset) + 12 > uint64_t(section_size + 2)) {
                    return false;
                }
                auto ifd_main_entries_count = buffer.read_int<uint16_t>(first_ifd_offset + 10, big_endian);
                for (uint16_t i = 0; i < ifd_main_entries_count; ++i) {
                    off_t entry_offset = first_ifd_offset + 12 + i * 12;
                    if (entry_offset + 12 > section_size + 2) {
                        return false;
                    }
                    auto tag = buffer.read_int<uint16_t>(entry_offset, big_endian);
                    if (tag == 274) {  // Orientation Tag
                        orientation = buffer.read_int<uint16_t>(entry_offset + 8, big_endian);
                    }
                }
            }
            offset += section_size + 2;
            continue;
        }

        // 0xFFC0 is baseline standard (SOF0)
        // 0xFFC1 is baseline optimized (SOF1)
        // 0xFFC2 is progressive (SOF2)
        if (buffer.cmp_any_of(0, 2, {"\xFF\xC0", "\xFF\xC1", "\xFF\xC2"})) {
            info = ImageInfo(kFormatJpeg, "jpg", "jpeg", "image/jpeg");
            ImageSize size(buffer.read_u16_be(7), buffer.read_u16_be(5));
            if (orientation == 5 || orientation == 6 || orientation == 7 || orientation == 8) {
                std::swap(size.width, size.height);
            }
            info.set_size(size);
            return true;
        }
        offset += section_size + 2;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// https://www.khronos.org/registry/KTX/specs/1.0/ktxspec_v1.html
inline bool try_ktx(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 44) {
        return false;
    }
    auto buffer = ri.read_buffer(0, 44);
    if (!buffer.cmp(0, 12, "\xABKTX 11\xBB\r\n\x1A\n")) {
        return false;
    }

    info = ImageInfo(kFormatKtx, "ktx", "ktx", "image/ktx");
    info.set_size(               //
        buffer.read_u32_le(36),  //
        buffer.read_u32_le(40)   //
    );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// https://www.fileformat.info/format/png/corion.htm
inline bool try_png(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 24) {
        return false;
    }

    auto buffer = ri.read_buffer(0, std::min<size_t>(length, 40));
    if (!buffer.cmp(0, 4, "\x89PNG")) {
        return false;
    }

    std::string first_chunk_type = buffer.read_string(12, 4);
    if (first_chunk_type == "IHDR") {
        info = ImageInfo(kFormatPng, "png", "png", "image/png");
        info.set_size(               //
            buffer.read_u32_be(16),  //
            buffer.read_u32_be(20)   //
        );
        return true;
    } else if (first_chunk_type == "CgBI") {
        if (buffer.size() >= 40 && buffer.read_string(28, 4) == "IHDR") {
            info = ImageInfo(kFormatPng, "png", "png", "image/png");
            info.set_size(               //
                buffer.read_u32_be(32),  //
                buffer.read_u32_be(36)   //
            );
            return true;
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool try_psd(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 22) {
        return false;
    }
    auto buffer = ri.read_buffer(0, 22);
    if (!buffer.cmp(0, 6, "8BPS\x00\x01")) {
        return false;
    }

    info = ImageInfo(kFormatPsd, "psd", "psd", "image/psd");
    info.set_size(               //
        buffer.read_u32_be(18),  //
        buffer.read_u32_be(14)   //
    );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool try_qoi(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 12) {
        return false;
    }
    auto buffer = ri.read_buffer(0, 12);
    if (!buffer.cmp(0, 4, "qoif")) {
        return false;
    }

    info = ImageInfo(kFormatQoi, "qoi", "qoi", "image/qoi");
    info.set_size(              //
        buffer.read_u32_be(4),  //
        buffer.read_u32_be(8)   //
    );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// https://www.fileformat.info/format/tiff/corion.htm
inline bool try_tiff(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 8) {
        return false;
    }
    auto buffer = ri.read_buffer(0, 8);
    if (!buffer.cmp_any_of(0, 4, {"\x49\x49\x2A\x00", "\x4D\x4D\x00\x2A"})) {
        return false;
    }

    bool swap_endian = buffer[0] == 0x4D;

    auto offset = buffer.read_int<uint32_t>(4, swap_endian);
    if (uint64_t(length) < uint64_t(offset) + 2) {
        return false;
    }

    buffer = ri.read_buffer(offset, 2);

    auto num_entry = buffer.read_int<uint16_t>(0, swap_endian);
    offset += 2;

    int64_t width = -1;
    int64_t height = -1;
    for (uint16_t i = 0; i < num_entry && length >= offset + 12 && (width == -1 || height == -1); ++i, offset += 12) {
        if (offset + 12 > length) {
            return false;
        }
        buffer = ri.read_buffer(offset, 12);

        auto tag = buffer.read_int<uint16_t>(0, swap_endian);
        auto type = buffer.read_int<uint16_t>(2, swap_endian);

        if (tag == 256) {  // Found ImageWidth entry
            if (type == 3) {
                width = buffer.read_int<uint16_t>(8, swap_endian);
            } else if (type == 4) {
                width = buffer.read_int<uint32_t>(8, swap_endian);
            }
        } else if (tag == 257) {  // Found ImageHeight entry
            if (type == 3) {
                height = buffer.read_int<uint16_t>(8, swap_endian);
            } else if (type == 4) {
                height = buffer.read_int<uint32_t>(8, swap_endian);
            }
        }
    }

    bool ok = width != -1 && height != -1;
    if (ok) {
        info = ImageInfo(kFormatTiff, "tiff", "tiff", "image/tiff");
        info.set_size(width, height);
    }
    return ok;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// https://developers.google.com/speed/webp/docs/riff_container
inline bool try_webp(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 16) {
        return false;
    }
    auto buffer = ri.read_buffer(0, std::min<size_t>(length, 30));
    if (!buffer.cmp(0, 4, "RIFF") || !buffer.cmp(8, 4, "WEBP")) {
        return false;
    }

    std::string type = buffer.read_string(12, 4);
    if (type == "VP8 " && buffer.size() >= 30) {
        info = ImageInfo(kFormatWebp, "webp", "webp", "image/webp");
        info.set_size(                        //
            buffer.read_u16_le(26) & 0x3FFF,  //
            buffer.read_u16_le(28) & 0x3FFF   //
        );
        return true;
    } else if (type == "VP8L" && buffer.size() >= 25) {
        uint32_t n = buffer.read_u32_le(21);
        info = ImageInfo(kFormatWebp, "webp", "webp", "image/webp");
        info.set_size(                //
            (n & 0x3FFF) + 1,         //
            ((n >> 14) & 0x3FFF) + 1  //
        );
        return true;
    } else if (type == "VP8X" && buffer.size() >= 30) {
        uint8_t extended_header = buffer.read_u8(20);
        bool valid_start = (extended_header & 0xc0) == 0;
        bool valid_end = (extended_header & 0x01) == 0;
        if (valid_start && valid_end) {
            info = ImageInfo(kFormatWebp, "webp", "webp", "image/webp");
            info.set_size(                                        //
                (buffer.read_u32_le(24) & 0x00FFFFFF) + 1,        //
                ((buffer.read_u32_le(26) & 0xFFFFFF00) >> 8) + 1  //
            );
            return true;
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO Not rigorous enough, keep it as last detector
// https://www.fileformat.info/format/tga/corion.htm
inline bool try_tga(ReadInterface &ri, size_t length, ImageInfo &info) {
    if (length < 18) {
        return false;
    }

    auto buffer = ri.read_buffer((off_t)(length - 18), 18);

    if (buffer.cmp(0, 18, "TRUEVISION-XFILE.\x00")) {
        if (length < 18 + 16) {
            return false;
        }
        buffer = ri.read_buffer(0, 18);
        info = ImageInfo(kFormatTga, "tga", "tga", "image/tga");
        info.set_size(               //
            buffer.read_u16_le(12),  //
            buffer.read_u16_le(14)   //
        );
        return true;
    }

    buffer = ri.read_buffer(0, 18);

    uint8_t id_len = buffer.read_u8(0);
    if (length < (size_t)id_len + 18) {
        return false;
    }

    uint8_t color_map_type = buffer.read_u8(1);
    uint8_t image_type = buffer.read_u8(2);
    uint16_t first_color_map_entry_index = buffer.read_u16_le(3);
    uint16_t color_map_length = buffer.read_u16_le(5);
    uint8_t color_map_entry_size = buffer.read_u8(7);
    // uint16_t x_origin = buffer.read_u16_le(8);
    // uint16_t y_origin = buffer.read_u16_le(10);
    uint16_t w = buffer.read_u16_le(12);
    uint16_t h = buffer.read_u16_le(14);
    // uint8_t pixel_depth = buffer.read_u8(16);
    // uint8_t flags = buffer.read_u8(17);

    if (color_map_type == 0) {  // no color map
        if (image_type == 0 || image_type == 2 || image_type == 3 || image_type == 10 || image_type == 11 ||
            image_type == 32 || image_type == 33) {
            if (first_color_map_entry_index == 0 && color_map_length == 0 && color_map_entry_size == 0) {
                info = ImageInfo(kFormatTga, "tga", "tga", "image/tga");
                info.set_size(w, h);
                return true;
            }
        }
    } else if (color_map_type == 1) {  // 256 entry palette
        if (image_type == 1 || image_type == 9) {
            info = ImageInfo(kFormatTga, "tga", "tga", "image/tga");
            info.set_size(w, h);
            return true;
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum DetectorIndex {
    kDetectorIndexAvifHeic = 0,
    kDetectorIndexBmp,
    kDetectorIndexCurIco,
    kDetectorIndexDds,
    kDetectorIndexGif,
    kDetectorIndexHdr,
    kDetectorIndexIcns,
    kDetectorIndexJpeg2000CodeStream,
    kDetectorIndexJpeg2000,
    kDetectorIndexJpg,
    kDetectorIndexKtx,
    kDetectorIndexPng,
    kDetectorIndexPsd,
    kDetectorIndexQoi,
    kDetectorIndexTiff,
    kDetectorIndexWebp,
    kDetectorIndexTga,
    //
    DETECTOR_COUNT
};

using Detector = bool (*)(ReadInterface &ri, size_t length, ImageInfo &info);

template <typename T, size_t N>
inline constexpr size_t countof(T (&)[N]) noexcept {
    return N;
}

struct DetectorInfo {
    Format format;
    DetectorIndex index;
    Detector detect;
};

template <size_t N, int I = N - 1>
struct check_format_order_ {
    static constexpr bool check(const DetectorInfo (&dl)[N]) {
        return (dl[I].format == static_cast<Format>(I + 1)) && check_format_order_<N, I - 1>::check(dl);
    }
};

template <size_t N>
struct check_format_order_<N, 0> {
    static constexpr bool check(const DetectorInfo (&dl)[N]) { return dl[0].format == static_cast<Format>(0 + 1); }
};

template <size_t N>
constexpr bool check_format_order(const DetectorInfo (&dl)[N]) {
    return check_format_order_<N>::check(dl);
}

inline ImageInfo parse(ReadInterface &ri,                               //
                       Format most_likely_format,                       //
                       const std::vector<Format> &likely_formats = {},  //
                       bool must_be_one_of_likely_formats = false) {    //
    size_t length = ri.length();

    constexpr DetectorInfo dl[] = {
        {kFormatAvif,           kDetectorIndexAvifHeic,            try_avif_heic},
        { kFormatBmp,                kDetectorIndexBmp,                  try_bmp},
        { kFormatCur,             kDetectorIndexCurIco,              try_cur_ico},
        { kFormatDds,                kDetectorIndexDds,                  try_dds},
        { kFormatGif,                kDetectorIndexGif,                  try_gif},
        { kFormatHdr,                kDetectorIndexHdr,                  try_hdr},
        {kFormatHeic,           kDetectorIndexAvifHeic,            try_avif_heic},
        {kFormatIcns,               kDetectorIndexIcns,                 try_icns},
        { kFormatIco,             kDetectorIndexCurIco,              try_cur_ico},
        { kFormatJ2k, kDetectorIndexJpeg2000CodeStream, try_jpeg2000_code_stream},
        { kFormatJp2,           kDetectorIndexJpeg2000,             try_jpeg2000},
        { kFormatJph,           kDetectorIndexJpeg2000,             try_jpeg2000},
        { kFormatJpx,           kDetectorIndexJpeg2000,             try_jpeg2000},
        {kFormatJpeg,                kDetectorIndexJpg,                  try_jpg},
        { kFormatKtx,                kDetectorIndexKtx,                  try_ktx},
        { kFormatPng,                kDetectorIndexPng,                  try_png},
        { kFormatPsd,                kDetectorIndexPsd,                  try_psd},
        { kFormatQoi,                kDetectorIndexQoi,                  try_qoi},
        {kFormatTiff,               kDetectorIndexTiff,                 try_tiff},
        {kFormatWebp,               kDetectorIndexWebp,                 try_webp},
        { kFormatTga,                kDetectorIndexTga,                  try_tga},
    };
    static_assert(FORMAT_COUNT == countof(dl), "FORMAT_COUNT != countof(dl)");
    static_assert(check_format_order(dl), "Format order is incorrect");

    bool tried[DETECTOR_COUNT] = {false};

    ImageInfo info;

    if (most_likely_format != Format::kFormatUnknown) {
        auto detector = dl[most_likely_format - 1];
        if (detector.detect(ri, length, info)  //
            && (!must_be_one_of_likely_formats || info.format() == most_likely_format)) {
            return info;
        }
        tried[detector.index] = true;
    }

    for (auto format : likely_formats) {
        if (format == Format::kFormatUnknown) {
            continue;
        }
        auto detector = dl[format - 1];
        if (tried[detector.index]) {
            continue;
        }
        if (detector.detect(ri, length, info)  //
            && (!must_be_one_of_likely_formats || info.format() == format)) {
            return info;
        }
        tried[detector.index] = true;
    }

    if (must_be_one_of_likely_formats) {
        return ImageInfo(kUnrecognizedFormat);
    }

    for (auto &detector : dl) {
        if (tried[detector.index]) {
            continue;
        }
        if (detector.detect(ri, length, info)) {
            return info;
        }
        tried[detector.index] = true;
    }

    return ImageInfo(kUnrecognizedFormat);
}

inline ImageInfo parse(ReadInterface &ri,                               //
                       const std::vector<Format> &likely_formats = {},  //
                       bool must_be_one_of_likely_formats = false) {    //
    return parse(ri, Format::kFormatUnknown, likely_formats, must_be_one_of_likely_formats);
}

template <typename ReaderType, typename InputType>
inline ImageInfo parse(const InputType &input,                          //
                       Format most_likely_format,                       //
                       const std::vector<Format> &likely_formats = {},  //
                       bool must_be_one_of_likely_formats = false) {    //
    ReaderType reader(input);
    size_t length = reader.size();
    ReadFunc read_func = [&reader](void *buf, off_t offset, size_t size) { reader.read(buf, offset, size); };
    ReadInterface ri(read_func, length);
    return parse(ri, most_likely_format, likely_formats, must_be_one_of_likely_formats);
}

template <typename ReaderType, typename InputType>
inline ImageInfo parse(const InputType &input,                          //
                       const std::vector<Format> &likely_formats = {},  //
                       bool must_be_one_of_likely_formats = false) {    //
    return parse<ReaderType>(input, Format::kFormatUnknown, likely_formats, must_be_one_of_likely_formats);
}

}  // namespace imageinfo

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif  // IMAGEINFO_IMAGEINFO_H
