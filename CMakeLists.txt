cmake_minimum_required(VERSION 3.10)
project(DynamicArray LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(main
    src/main.cpp
    src/DynamicArray.cpp
)
target_include_directories(main PRIVATE include)

if(MSVC)
    target_compile_options(main PRIVATE /W4 /utf-8)
else()
    target_compile_options(main PRIVATE -Wall -Wextra)
endif()
