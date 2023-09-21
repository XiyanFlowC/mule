add_library (sislua "LuaHost.h" "LuaHost.cpp" "LuaDataHandler.h" "LuaDataHandler.cpp" "LuaEnvironment.h" "LuaEnvironment.cpp" "Api/ContainerOperation.cpp" "Api/ContainerOperation.h" "Api/StreamOperation.cpp" "Api/StreamOperation.h")

target_link_libraries(sislua PUBLIC bis)
target_link_libraries(sislua PUBLIC lua)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET sislua PROPERTY CXX_STANDARD 20)
endif()
