#include <cstdio>
#include "imageinfo.hpp"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <file>...\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        const char *file = argv[i];
        auto info = getImageInfo<IIFilePathReader>(file);
        printf("File: %s\n", file);
        if (info.getErrorCode() != II_ERR_OK) {
            printf("  - Error    : %s\n", info.getErrorMsg());
        } else {
            printf("  - Format   : %d\n", info.getFormat());
            printf("  - Ext      : %s\n", info.getExt());
            printf("  - Full Ext : %s\n", info.getFullExt());
            printf("  - Size     : {width: %lld, height: %lld}\n", info.getWidth(), info.getHeight());
            printf("  - Mimetype : %s\n", info.getMimetype());
            printf("  - Entries  :\n");
            for(const auto &entrySize : info.getEntrySizes()) {
                printf("    - {width: %lld, height: %lld}\n", entrySize[0], entrySize[1]);
            }
        }
    }

    return 0;
}
