add_library (advstr SHARED "advstr.cpp" "advstr.h" "alstr.h" "alstr.cpp")

target_link_libraries (advstr PRIVATE mulert xybase_dyn)

if (MSVC)
	target_compile_options (advstr PRIVATE "/utf-8")
endif()

set_property(TARGET advstr PROPERTY CXX_STANDARD 20)