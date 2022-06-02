cmake_minimum_required(VERSION 3.2)
project(imageinfo)

set(CMAKE_CXX_STANDARD 11)

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)
endif ()

add_executable(imageinfo main.cpp imageinfo.hpp)
set_property(TARGET imageinfo PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

enable_testing()

add_executable(test_files test_files.cpp imageinfo.hpp)
set_property(TARGET test_files PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_compile_definitions(test_files PRIVATE -DIMAGES_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/images/")

add_test(NAME test_files COMMAND test_files)

if (CMAKE_CONFIGURATION_TYPES)
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
            --force-new-ctest-process --output-on-failure
            --build-config "$<CONFIGURATION>")
else ()
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
            --force-new-ctest-process --output-on-failure)
endif ()
