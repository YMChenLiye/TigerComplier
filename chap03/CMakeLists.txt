# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (parse)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)


#add_custom_command(
#    OUTPUT lex.yy.c
#    COMMAND flex tiger.lex
#    DEPENDS tiger.lex
#)

#add_custom_target(some_name ALL
#    DEPENDS lex.yy.c
#)

# 指定生成目标
add_executable(parse ${DIR_SRCS})

