cmake_minimum_required(VERSION 3.2)
project(imageinfo)

set(CMAKE_CXX_STANDARD 11)

#if (MSVC)
#    set(CompilerFlags
#            CMAKE_CXX_FLAGS
#            CMAKE_CXX_FLAGS_DEBUG
#            CMAKE_CXX_FLAGS_RELEASE
#            CMAKE_CXX_FLAGS_MINSIZEREL
#            CMAKE_CXX_FLAGS_RELWITHDEBINFO
#            CMAKE_C_FLAGS
#            CMAKE_C_FLAGS_DEBUG
#            CMAKE_C_FLAGS_RELEASE
#            CMAKE_C_FLAGS_MINSIZEREL
#            CMAKE_C_FLAGS_RELWITHDEBINFO)
#    foreach(CompilerFlag ${CompilerFlags})
#        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
#        set(${CompilerFlag} "${${CompilerFlag}}" CACHE STRING "msvc compiler flags" FORCE)
#    endforeach()
#endif ()

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)
endif ()

add_executable(imageinfo main.cpp imageinfo.hpp)

enable_testing()

add_executable(test_files test_files.cpp imageinfo.hpp)

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
