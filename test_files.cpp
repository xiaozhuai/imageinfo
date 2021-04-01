//
// Created by xiaozhuai on 2021/4/1.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include "imageinfo.hpp"

int get_image_info(const std::string &file) {
    ImageInfo<const std::string &, IIFilePathReader> imageInfo(file);
    std::cout << "File: " << file << "\n";
    std::cout << "  - Error    : " << imageInfo.getErrorMsg() << "\n";
    std::cout << "  - Width    : " << imageInfo.getWidth() << "\n";
    std::cout << "  - Height   : " << imageInfo.getHeight() << "\n";
    std::cout << "  - Format   : " << imageInfo.getFormat() << "\n";
    std::cout << "  - Ext      : " << imageInfo.getExt() << "\n";
    std::cout << "  - Full Ext : " << imageInfo.getFullExt() << "\n";
    std::cout << "  - Mimetype : " << imageInfo.getMimetype() << "\n\n";
    return imageInfo.getErrorCode();
}

std::vector<std::string> parse_files(const std::string &file) {
    std::fstream in(file, std::ios::in);
    assert(in.is_open());

    std::stringstream ss;
    ss << in.rdbuf();
    in.close();

    std::vector<std::string> lines;
    std::string line;
    while (!ss.eof()) {
        std::getline(ss, line);
        if (line.empty()) continue;
        lines.emplace_back(line);
    }

    assert(!lines.empty());
    return lines;
}

int main() {
    {
        auto images = parse_files(VALID_FILES_PATH);
        for (auto &image : images) {
            assert(get_image_info(image) == II_ERR_OK);
        }
    }

    {
        auto images = parse_files(INVALID_FILES_PATH);
        for (auto &image : images) {
            assert(get_image_info(image) != II_ERR_OK);
        }
    }

    return 0;
}