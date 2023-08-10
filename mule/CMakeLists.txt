add_executable (mule "mule.cpp" "mule.h" "Configuration.cpp" "Configuration.h" "ShiftableReferrence.cpp" "ShiftableReferrence.h" "ElfStream.h" "ElfStream.cpp" "ResourceManager.h" "ResourceManager.cpp" "crc32.h" "crc32.cpp" "BinaryBlock.h" "BinaryBlock.cpp" "IsoContainer.cpp" "IsoContainer.h" "luaenv.cpp" "luaenv.h")

target_link_libraries (mule PRIVATE sis sislua)
target_include_directories (mule PRIVATE "${PROJECT_SOURCE_DIR}/sis" "${PROJECT_SOURCE_DIR}/bis")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET mule PROPERTY CXX_STANDARD 20)
endif()
