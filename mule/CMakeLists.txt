add_executable (mule "mule.cpp" "mule.h"   "ShiftableString.cpp" "ShiftableString.h"     "crc32.h" "crc32.cpp"     "luaenv.cpp" "luaenv.h" "codepage.h" "codepage.cpp")

target_link_libraries (mule PRIVATE sis sislua)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET mule PROPERTY CXX_STANDARD 20)
endif()
