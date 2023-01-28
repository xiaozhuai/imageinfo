cmake_minimum_required(VERSION 3.2)
project(imageinfo)

set(CMAKE_CXX_STANDARD 11)

option(IMAGEINFO_BUILD_TOOL "Build tool" ON)
option(IMAGEINFO_BUILD_TESTS "Build tests" ON)

add_library(imageinfo INTERFACE)
add_library(imageinfo::imageinfo ALIAS imageinfo)

include(GNUInstallDirs)

target_include_directories(imageinfo INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if (IMAGEINFO_BUILD_TOOL)
    add_executable(imageinfo_cli main.cpp imageinfo.hpp)
    set_target_properties(imageinfo_cli PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
            OUTPUT_NAME "imageinfo")
endif ()

if (IMAGEINFO_BUILD_TESTS)
    enable_testing()

    add_executable(imageinfo_tests tests.cpp imageinfo.hpp)
    set_target_properties(imageinfo_tests PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_compile_definitions(imageinfo_tests PRIVATE -DIMAGES_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/images/")
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

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    install(TARGETS imageinfo EXPORT imageinfo)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/imageinfo.hpp" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(EXPORT imageinfo FILE imageinfo-config.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/imageinfo NAMESPACE imageinfo::)
    if (TARGET imageinfo_cli)
        install(TARGETS imageinfo_cli)
    endif ()
endif ()
