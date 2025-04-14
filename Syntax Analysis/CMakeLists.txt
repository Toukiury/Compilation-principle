cmake_minimum_required(VERSION 3.26)
project(pascal_c)

set(CMAKE_CXX_STANDARD 23)

# 查找 Flex 和 Bison
find_program(FLEX_EXECUTABLE NAMES flex PATHS "E:/by/bison_flex_mingw/GnuWin32/bin")
find_program(BISON_EXECUTABLE NAMES bison PATHS "E:/by/bison_flex_mingw/GnuWin32/bin")

if(NOT FLEX_EXECUTABLE)
    message(FATAL_ERROR "Flex not found")
endif()
if(NOT BISON_EXECUTABLE)
    message(FATAL_ERROR "Bison not found")
endif()

# 生成 lex.yy.c
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/lex.yy.c
        COMMAND ${FLEX_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# 生成 parser.tab.c 和 parser.tab.h
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/parser.tab.c ${CMAKE_CURRENT_SOURCE_DIR}/parser.tab.h
        COMMAND ${BISON_EXECUTABLE} -d ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)



add_executable(pascal_c lex.yy.c parser.tab.c main.c)
target_include_directories(pascal_c PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})