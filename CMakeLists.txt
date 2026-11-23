cmake_minimum_required(VERSION 3.10)
project(DynamicArray LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Шаблонный класс полностью реализован в include/DynamicArray.h,
# поэтому компилируется только main.cpp.
add_executable(main
    src/main.cpp
)
target_include_directories(main PRIVATE include)

if(MSVC)
    target_compile_options(main PRIVATE /W4 /utf-8)
else()
    target_compile_options(main PRIVATE -Wall -Wextra)
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(dynamic_array_tests tests/dynamic_array_tests.cpp)
    target_include_directories(dynamic_array_tests PRIVATE include)
    add_test(NAME dynamic_array_tests COMMAND dynamic_array_tests)
endif()
