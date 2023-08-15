
set(LUA_VERSION 5.4.4)

add_library(lua "lua/lapi.c" "lua/lcode.c" "lua/lctype.c" "lua/ldebug.c" "lua/ldo.c" "lua/ldump.c" "lua/lfunc.c" "lua/lgc.c" "lua/llex.c" "lua/lmem.c" "lua/lobject.c" "lua/lopcodes.c" "lua/lparser.c" "lua/lstate.c" "lua/lstring.c" "lua/ltable.c" "lua/ltm.c" "lua/lundump.c" "lua/lvm.c" "lua/lzio.c" "lua/lauxlib.c" "lua/lbaselib.c" "lua/lcorolib.c" "lua/ldblib.c" "lua/liolib.c" "lua/lmathlib.c" "lua/loadlib.c" "lua/loslib.c" "lua/lstrlib.c" "lua/ltablib.c" "lua/lutf8lib.c" "lua/linit.c")
target_include_directories (lua INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include/lua")

add_subdirectory("xybase")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET lua PROPERTY CXX_STANDARD 20)
endif()