﻿# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("mule" VERSION 0.0.0)

# 包含子项目。
add_subdirectory ("lib")
add_subdirectory ("bis")
add_subdirectory ("sis")
add_subdirectory ("mule")

file (COPY "./scripts" DESTINATION "${CMAKE_BINARY_DIR}/mule")
