# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.12)

project ("mule" VERSION 1.7.0)

set (USING_LUA ON)

set (CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure all outputs go to the same directory for proper plugin loading
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Platform-specific configurations
if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  # 使用 ZI 支持热重载
  set (CMAKE_CXX_FLAGS_DEBUG "/MDd /ZI /Ob0 /Od /RTC1")
elseif(UNIX AND NOT APPLE)
  # Linux-specific settings
  set(LINUX TRUE)
  add_definitions(-DLINUX)
  # Enable position independent code for shared libraries
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  # Add rpath to find libraries in the same directory as executable
  set(CMAKE_INSTALL_RPATH "$ORIGIN")
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_options("-Wall" "-Wextra" "-g")
  endif()
  # Enable format library support for GCC < 13
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "13")
    link_libraries(fmt)
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

# Installation configuration
include (GNUInstallDirs)

# Install binaries - all to the same directory for plugin compatibility
install(TARGETS advstr DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS mule DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS mulert DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install shared libraries to the same directory as executables
install(TARGETS xybase_dyn DESTINATION ${CMAKE_INSTALL_BINDIR})
if(TARGET mulert)
  install(TARGETS mulert DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# Platform-specific compiler options
if(MSVC)
  target_compile_options(sislua PUBLIC "/utf-8")
  target_compile_options(bis PUBLIC "/utf-8")
  target_compile_options(sis PUBLIC "/utf-8")
  target_compile_options(mule PUBLIC "/utf-8")
  if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_link_options(bis PUBLIC "/INCREMENTAL")
    target_link_options(sis PUBLIC "/INCREMENTAL")
    target_link_options(mule PUBLIC "/INCREMENTAL")
  endif()
endif()
