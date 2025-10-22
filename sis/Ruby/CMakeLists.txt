add_library (sisruby "RubyHost.cpp" "RubyHost.h")

target_link_libraries(sisruby PUBLIC bis)
target_include_directories(sisruby PUBLIC "${PROJECT_SOURCE_DIR}/bis")
target_link_libraries(sisruby PUBLIC lua)
target_include_directories(sisruby PUBLIC "${PROJECT_SOURCE_DIR}/lib/include/ruby")

# Ensure C++ standard
set_property(TARGET sisruby PROPERTY CXX_STANDARD 20)
set_property(TARGET sisruby PROPERTY CXX_STANDARD_REQUIRED ON)
