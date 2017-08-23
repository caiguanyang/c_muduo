cmake_minimum_required (VERSION 2.8)
# 设置 GCC, G++版本
set (CMAKE_C_COMPILER "/opt/compiler/gcc-4.8.2/bin/gcc")
set (CMAKE_CXX_COMPILER "/opt/compiler/gcc-4.8.2/bin/g++")

# 项目信息
project (muduo_cai)

# 设置头文件检索路径
include_directories ("${PROJECT_SOURCE_DIR}")
include_directories ("/home/users/caiguanyang/tools/boost/include")

# 生成 muduo 原生库
add_subdirectory (muduo/base)
add_subdirectory (muduo/net)
add_subdirectory (muduo/cainet)

## 指定生成的目标
#add_executable(test_1 muduo/test/t1.cpp)
#target_link_libraries (test_1 muduo_cainet)
#
## test_2
#add_executable(test_2 muduo/test/t2.cpp)
#target_link_libraries (test_2 muduo_base)
#target_link_libraries (test_2 muduo_cainet)

# test_3
# 设置链接参数
set (CMAKE_MODULE_LINKER_FLAGS "-lrt -ldl -lz")

add_executable(test_3 muduo/test/t3.cpp)
target_link_libraries (test_3 muduo_base)
target_link_libraries (test_3 muduo_cainet)
