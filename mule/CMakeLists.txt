add_executable (mule "mule.cpp" "mule.h"         "crc32.h" "crc32.cpp"     "luaenv.cpp" "luaenv.h" "codepage.h" "codepage.cpp" "version.h.in")

string (TIMESTAMP COMPILE_TIME %Y/%m/%d-%H:%M:%S)
set (BUILD_TIME ${COMPILE_TIME})
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/version.h")

target_link_libraries (mule PRIVATE sis sislua)

# Ensure C++ standard
set_property(TARGET mule PROPERTY CXX_STANDARD 20)
set_property(TARGET mule PROPERTY CXX_STANDARD_REQUIRED ON)

# Installation
include(GNUInstallDirs)
install(TARGETS mule DESTINATION ${CMAKE_INSTALL_BINDIR})
