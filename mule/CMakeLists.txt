add_executable (mule "mule.cpp" "mule.h" "Configuration.cpp" "Configuration.h" "ShiftableReferrence.cpp" "ShiftableReferrence.h"   "ResourceManager.h" "ResourceManager.cpp" "crc32.h" "crc32.cpp" "BinaryBlock.h" "BinaryBlock.cpp"   "luaenv.cpp" "luaenv.h")

target_link_libraries (mule PRIVATE sis sislua)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET mule PROPERTY CXX_STANDARD 20)
endif()
