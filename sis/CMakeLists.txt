
add_library (sis "Lua/LuaHost.h" "Lua/LuaHost.cpp" "Lua/LuaDataHandler.h" "Lua/LuaDataHandler.cpp" "Lua/LuaApi.h" "Lua/LuaApi.cpp" "Ruby/RubyHost.cpp" "Ruby/RubyHost.h" "Cpp/StructureBuilder.h" "Cpp/StructureBuilder.cpp")

target_link_libraries(sis PUBLIC bis)
target_include_directories(sis PUBLIC "${PROJECT_SOURCE_DIR}/bis")

target_link_libraries(sis PUBLIC liblua)
target_include_directories(sis PUBLIC "${PROJECT_SOURCE_DIR}/lib/include/lua")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET sis PROPERTY CXX_STANDARD 20)
endif()
