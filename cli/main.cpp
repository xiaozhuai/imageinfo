#include <cstdio>

#include "imageinfo.hpp"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s [FILE]...\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        const char *file = argv[i];
        auto info = imageinfo::parse<imageinfo::FilePathReader>(file);
        printf("File: %s\n", file);
        if (!info) {
            printf("  - Error    : %s\n", info.error_msg());
        } else {
            printf("  - Format   : %d\n", info.format());
            printf("  - Ext      : %s\n", info.ext());
            printf("  - Full Ext : %s\n", info.full_ext());
            printf("  - Size     : {width: %lld, height: %lld}\n", info.size().width, info.size().height);
            printf("  - Mimetype : %s\n", info.mimetype());
            if (!info.entry_sizes().empty()) {
                printf("  - Entries  :\n");
                for (const auto &entrySize : info.entry_sizes()) {
                    printf("    - {width: %lld, height: %lld}\n", entrySize[0], entrySize[1]);
                }
            }
        }
    }

    return 0;
}
