cmake_minimum_required(VERSION 3.15)
project(imageinfo)

set(CMAKE_CXX_STANDARD 11)

set(IMAGEINFO_IS_MASTER_PROJECT OFF)
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    set(IMAGEINFO_IS_MASTER_PROJECT ON)
endif ()

option(IMAGEINFO_BUILD_TOOL "Build tool" ${IMAGEINFO_IS_MASTER_PROJECT})
option(IMAGEINFO_BUILD_TESTS "Build tests" ${IMAGEINFO_IS_MASTER_PROJECT})
option(IMAGEINFO_BUILD_INSTALL "Build install" ${IMAGEINFO_IS_MASTER_PROJECT})

add_library(imageinfo INTERFACE)
add_library(imageinfo::imageinfo ALIAS imageinfo)

include(GNUInstallDirs)

target_include_directories(imageinfo INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

if (IMAGEINFO_BUILD_TOOL)
    add_executable(imageinfo_cli main.cpp)
    target_link_libraries(imageinfo_cli PRIVATE imageinfo)
    set_target_properties(imageinfo_cli PROPERTIES OUTPUT_NAME "imageinfo")
endif ()

if (IMAGEINFO_BUILD_TESTS)
    enable_testing()

    add_executable(imageinfo_tests tests.cpp imageinfo.hpp)
    target_compile_definitions(imageinfo_tests PRIVATE
            -DIMAGES_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/images/"
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
            FILES "${CMAKE_CURRENT_SOURCE_DIR}/imageinfo.hpp"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
    install(
            EXPORT imageinfo
            FILE imageinfo-config.cmake
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/imageinfo"
            NAMESPACE imageinfo::
    )
    if (IMAGEINFO_BUILD_TOOL)
        install(TARGETS imageinfo_cli)
    endif ()
endif ()
