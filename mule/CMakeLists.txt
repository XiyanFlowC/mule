add_executable (mule "mule.cpp" "mule.h"         "crc32.h" "crc32.cpp"     "luaenv.cpp" "luaenv.h" "codepage.h" "codepage.cpp" "version.h.in")

set (mule_MAJOR_VERSION 1)
set (mule_MINOR_VERSION 5)

string (TIMESTAMP COMPILE_TIME %Y/%m/%d-%H:%M:%S)
set (BUILD_TIME ${COMPILE_TIME})
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/version.h")

target_link_libraries (mule PRIVATE sis sislua)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET mule PROPERTY CXX_STANDARD 20)
endif()
