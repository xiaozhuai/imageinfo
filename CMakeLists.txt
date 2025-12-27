cmake_minimum_required(VERSION 3.15)
project(imageinfo)

set(CMAKE_CXX_STANDARD 11)

set(IMAGEINFO_IS_MASTER_PROJECT OFF)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    set(IMAGEINFO_IS_MASTER_PROJECT ON)
endif ()

option(IMAGEINFO_BUILD_TOOLS "Build tools" ${IMAGEINFO_IS_MASTER_PROJECT})
option(IMAGEINFO_BUILD_TESTS "Build tests" ${IMAGEINFO_IS_MASTER_PROJECT})
option(IMAGEINFO_BUILD_INSTALL "Build install" ${IMAGEINFO_IS_MASTER_PROJECT})

add_library(imageinfo INTERFACE)
add_library(imageinfo::imageinfo ALIAS imageinfo)

include(GNUInstallDirs)

target_include_directories(imageinfo INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

if (IMAGEINFO_BUILD_TOOLS)
    add_executable(imageinfo_cli cli/main.cpp)
    target_link_libraries(imageinfo_cli PRIVATE imageinfo)
    set_target_properties(imageinfo_cli PROPERTIES OUTPUT_NAME "imageinfo")
endif ()

if (IMAGEINFO_BUILD_TESTS)
    enable_testing()

    add_executable(imageinfo_tests tests/tests.cpp)
    target_link_libraries(imageinfo_tests PRIVATE imageinfo)
    target_compile_definitions(imageinfo_tests PRIVATE
            -DIMAGES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/images/"
    )
    add_test(NAME imageinfo_tests COMMAND imageinfo_tests)

    if (CMAKE_CONFIGURATION_TYPES)
        add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                --force-new-ctest-process --output-on-failure
                --build-config "$<CONFIGURATION>")
    else ()
        add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                --force-new-ctest-process --output-on-failure)
    endif ()
endif ()

if (IMAGEINFO_BUILD_INSTALL)
    install(TARGETS imageinfo EXPORT imageinfo)
    install(
            FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/imageinfo.hpp"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
    install(
            EXPORT imageinfo
            FILE imageinfo-config.cmake
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/imageinfo"
            NAMESPACE imageinfo::
    )
    if (IMAGEINFO_BUILD_TOOLS)
        install(TARGETS imageinfo_cli)
    endif ()
endif ()

if (IMAGEINFO_IS_MASTER_PROJECT)
    find_program(CLANG_FORMAT clang-format NO_CMAKE_PATH)
    if (CLANG_FORMAT)
        set(ALL_SOURCES include/imageinfo.hpp cli/main.cpp tests/tests.cpp)
        add_custom_target(
                format
                COMMAND "${CLANG_FORMAT}" -i --verbose ${ALL_SOURCES}
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        add_custom_target(
                check-format
                COMMAND "${CLANG_FORMAT}" --dry-run --Werror --verbose ${ALL_SOURCES}
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        )
    endif ()
endif ()
