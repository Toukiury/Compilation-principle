cmake_minimum_required(VERSION 3.10)

project(PASCC)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)

# 包含当前目录
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# 指定文件输出位置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_EXPORT_COMPILECOMMANDS ON)

# 收集所有cpp文件
FILE(GLOB_RECURSE SRC_FILES
    ${PROJECT_SOURCE_DIR}/ast/*.cpp
    ${PROJECT_SOURCE_DIR}/another/*.cpp
    ${PROJECT_SOURCE_DIR}/ir_opt/*.cpp
    ${PROJECT_SOURCE_DIR}/lexical_pasrser/*.cpp
)

# 主源文件
SET(MAIN_SRC ${PROJECT_SOURCE_DIR}/main.cpp)

# 仅构建pascc可执行文件
ADD_EXECUTABLE(pascc ${SRC_FILES} ${MAIN_SRC})

# 添加调试信息
add_compile_options(-g)

# 添加线程支持
target_link_libraries(pascc -pthread) 