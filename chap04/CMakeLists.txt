# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (parse)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)


add_custom_command(
    OUTPUT lex.yy.c
    COMMAND flex ${CMAKE_SOURCE_DIR}/tiger.lex
    DEPENDS ${CMAKE_SOURCE_DIR}/tiger.lex
)

add_custom_command(
    OUTPUT y.tab.h y.tab.c
    COMMAND bison -ydv ${CMAKE_SOURCE_DIR}/tiger.grm
    DEPENDS ${CMAKE_SOURCE_DIR}/tiger.grm
)

include_directories(SYSTEM "${CMAKE_SOURCE_DIR}")

# 指定生成目标
add_executable(parse ${DIR_SRCS} lex.yy.c y.tab.c)

