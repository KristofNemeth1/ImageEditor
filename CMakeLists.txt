cmake_minimum_required(VERSION 3.23)

project(Composer
    VERSION 0.1.0
    LANGUAGES CXX
)

add_library(ComposerCore)

target_sources(ComposerCore
    PRIVATE
        src/Image.cpp
        src/Pixel.cpp
        src/BlendMode.cpp
        src/filter/Filter.cpp
    PUBLIC
        src/Image.hpp
        src/Pixel.hpp
        src/BlendMode.hpp
        src/filter/Filter.hpp
        src/stb_image.h
        src/stb_image_write.h
)

target_compile_features(ComposerCore PUBLIC cxx_std_17)

target_include_directories(ComposerCore
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

set_target_properties(ComposerCore
    PROPERTIES
        CXX_EXTENSIONS OFF
)

if(MSVC)
    target_compile_options(ComposerCore PRIVATE /W4)
    set_source_files_properties(src/Image.cpp
        PROPERTIES
            COMPILE_OPTIONS "/wd4244;/wd4996"
    )
else()
    target_compile_options(ComposerCore PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(Composer src/main.cpp)

target_link_libraries(Composer
    PRIVATE
        ComposerCore
)

set_target_properties(Composer
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

include(CTest)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)
    include(GoogleTest)

    add_executable(ComposerTests
        tests/FilterTests.cpp
    )

    target_link_libraries(ComposerTests
        PRIVATE
            ComposerCore
            GTest::gtest
            GTest::gtest_main
    )

    gtest_discover_tests(ComposerTests)
endif()
