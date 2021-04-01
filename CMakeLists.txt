cmake_minimum_required(VERSION 3.19)
project(imageinfo)

set(CMAKE_CXX_STANDARD 14)

add_executable(imageinfo main.cpp imageinfo.hpp)


enable_testing()

set(VALID_FILES_PATH "${CMAKE_CURRENT_BINARY_DIR}/valid_files.txt")
set(INVALID_FILES_PATH "${CMAKE_CURRENT_BINARY_DIR}/invalid_files.txt")

file(GLOB_RECURSE VALID_IMAGES images/valid/*)
file(WRITE "${VALID_FILES_PATH}" "")
foreach (file ${VALID_IMAGES})
    file(APPEND "${VALID_FILES_PATH}" "${file}\n")
endforeach ()

file(GLOB_RECURSE INVALID_IMAGES images/invalid/*)
file(WRITE "${INVALID_FILES_PATH}" "")
foreach (file ${INVALID_IMAGES})
    file(APPEND "${INVALID_FILES_PATH}" "${file}\n")
endforeach ()

add_executable(test_files test_files.cpp imageinfo.hpp)

target_compile_definitions(test_files PRIVATE -DVALID_FILES_PATH="${VALID_FILES_PATH}" -DINVALID_FILES_PATH="${INVALID_FILES_PATH}")

add_test(NAME test_files COMMAND test_files)
