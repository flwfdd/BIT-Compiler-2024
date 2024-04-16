cmake_minimum_required(VERSION 3.16)
project(lab02)
set(CMAKE_CXX_STANDARD 14)

# 使用 cmake 的 flex 模块
include(FindFLEX)
if(FLEX_FOUND)
 message("Info: flex found!")
else()
 message("Error: flex not found!")
endif()
# 为了 flex 新增头文件搜索路径
include_directories(${CMAKE_SOURCE_DIR})
# 指定 flex 编译目标
FLEX_TARGET(lexer lexer.flex ${CMAKE_CURRENT_BINARY_DIR}/../lexer.cpp)

add_compile_options(-pedantic)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
add_compile_options(-Wall)
add_compile_options(-Wextra)
add_executable(Compilerlab2
    main.cpp
    lexer.cpp
    parser.cpp
)
target_compile_features(Compilerlab2 PRIVATE cxx_std_14)