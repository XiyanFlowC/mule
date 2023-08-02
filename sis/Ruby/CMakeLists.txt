add_library (sisruby "RubyHost.cpp" "RubyHost.h")

target_link_libraries(sisruby PUBLIC bis)
target_include_directories(sisruby PUBLIC "${PROJECT_SOURCE_DIR}/bis")
target_link_libraries(sisruby PUBLIC liblua)
target_include_directories(sisruby PUBLIC "${PROJECT_SOURCE_DIR}/lib/include/ruby")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET sisruby PROPERTY CXX_STANDARD 20)
endif()
