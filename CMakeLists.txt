# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("mule" VERSION 0.0.0)

set (USING_LUA ON)

set (CXX_STANDARD_REQUIRED ON)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  # 使用 ZI 支持热重载
  set (CMAKE_CXX_FLAGS_DEBUG "/MDd /ZI /Ob0 /Od /RTC1 /Wall")
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
  if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_options("-Wall")
  endif()
 endif()

set (CMAKE_DEBUG_POSTFIX "d")

# 包含子项目。
add_subdirectory ("lib")
add_subdirectory ("mulert")
add_subdirectory ("bis")
add_subdirectory ("sis")
add_subdirectory ("mule")
add_subdirectory ("advstr")

if(MSVC)
  target_compile_options(sislua PUBLIC "/utf-8")
  target_compile_options(bis PUBLIC "/utf-8")
  target_compile_options(sis PUBLIC "/utf-8")
  target_compile_options(mule PUBLIC "/utf-8")
  if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd /ZI /Ob0 /Od /RTC1 /Wall")
    target_link_options(bis PUBLIC "/INCREMENTAL")
    target_link_options(sis PUBLIC "/INCREMENTAL")
    target_link_options(mule PUBLIC "/INCREMENTAL")
  endif()
endif()
