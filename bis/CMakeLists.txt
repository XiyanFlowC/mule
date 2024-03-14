add_library (bis   "Data/Basic/Integer.cpp"     "Data/Basic/String.cpp" "Data/Basic/String.h"             "Data/Basic/Float.cpp" "Data/Basic/Float.h" "Data/Basic/Double.cpp" "Data/Basic/Double.h"          "Container/BasicContainer.cpp" "Container/BasicContainer.h" "Data/VarChar.cpp" "Data/VarChar.h"     "Stream/ElfStream.h" "Stream/ElfStream.cpp" "Container/IsoContainer.cpp" "Container/IsoContainer.h"     "Data/Storage/BinaryBlock.h" "Data/Storage/BinaryBlock.cpp" "Data/BasicTypeCreator.h" "Data/BasicTypeCreator.cpp" "Data/Basic/Enum.cpp" "Data/Basic/Enum.h")

target_include_directories (bis INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries (bis PUBLIC mulert)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET bis PROPERTY CXX_STANDARD 20)
endif()
