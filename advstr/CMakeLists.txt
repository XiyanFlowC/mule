add_library (advstr SHARED "advstr.cpp" "advstr.h" "alstr.h" "alstr.cpp")

target_link_libraries (advstr PRIVATE mulert xybase_dyn)

# Platform-specific options
if (MSVC)
	target_compile_options (advstr PRIVATE "/utf-8")
elseif(UNIX)
	target_compile_options (advstr PRIVATE "-fPIC")
endif()

# Copy shared library to runtime directory on Linux
if(UNIX)
  add_custom_command(TARGET advstr POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:advstr> 
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

# Ensure C++ standard
set_property(TARGET advstr PROPERTY CXX_STANDARD 20)
set_property(TARGET advstr PROPERTY CXX_STANDARD_REQUIRED ON)

# Installation
include(GNUInstallDirs)
install(TARGETS advstr DESTINATION ${CMAKE_INSTALL_BINDIR})