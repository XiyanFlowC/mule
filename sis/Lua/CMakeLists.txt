add_library (sislua "LuaHost.h" "LuaHost.cpp" "LuaDataHandler.h" "LuaDataHandler.cpp" "LuaApi.h" "LuaApi.cpp")

target_link_libraries(sislua PUBLIC bis)
target_include_directories(sislua PUBLIC "${PROJECT_SOURCE_DIR}/bis")
target_link_libraries(sislua PUBLIC lua)
target_include_directories(sislua PUBLIC "${PROJECT_SOURCE_DIR}/lib/include/lua")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET sislua PROPERTY CXX_STANDARD 20)
endif()
