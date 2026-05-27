cmake_minimum_required(VERSION 3.23)

project(Composer
    VERSION 0.1.0
    LANGUAGES CXX
)

add_executable(Composer)

target_sources(Composer
    PRIVATE
        src/main.cpp
        src/Image.cpp
        src/Image.hpp
        src/Pixel.cpp
        src/Pixel.hpp
        src/BlendMode.cpp
        src/BlendMode.hpp
        src/filter/Filter.cpp
        src/filter/Filter.hpp
        src/stb_image.h
        src/stb_image_write.h
)

target_compile_features(Composer PRIVATE cxx_std_17)

target_include_directories(Composer
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

set_target_properties(Composer
    PROPERTIES
        CXX_EXTENSIONS OFF
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(MSVC)
    target_compile_options(Composer PRIVATE /W4)
    set_source_files_properties(src/Image.cpp
        PROPERTIES
            COMPILE_OPTIONS "/wd4244;/wd4996"
    )
else()
    target_compile_options(Composer PRIVATE -Wall -Wextra -Wpedantic)
endif()
