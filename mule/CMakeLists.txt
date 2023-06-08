add_executable (mule "mule.cpp" "mule.h" "Configuration.cpp" "Configuration.h")

target_link_libraries (mule PUBLIC sis)
target_include_directories (mule PUBLIC "${PROJECT_SOURCE_DIR}/sis" "${PROJECT_SOURCE_DIR}/bis")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET mule PROPERTY CXX_STANDARD 20)
endif()
