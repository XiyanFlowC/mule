add_library (advstr SHARED "advstr.cpp" "advstr.h" "alstr.h" "alstr.cpp")

target_link_libraries (advstr PRIVATE mulert xybase_dyn)

if (MSVC)
	target_compile_options (advstr PRIVATE "/utf-8")
endif()

if (LINUX)
  add_custom_command(TARGET advstr POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:advstr> 
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif ()

set_property(TARGET advstr PROPERTY CXX_STANDARD 20)