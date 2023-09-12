add_library (bis   "Data/Basic/Integer.cpp"     "Data/Basic/String.cpp" "Data/Basic/String.h"     "Data/Referrence.h"        "Data/Basic/Float.cpp" "Data/Basic/Float.h" "Data/Basic/Double.cpp" "Data/Basic/Double.h" "Data/Referrence.cpp" "Data/Table.cpp" "Data/Table.h"   "Data/Array.h" "Data/Array.cpp" "Data/Structure.cpp" "Data/Structure.h" "Container/BasicContainer.cpp" "Container/BasicContainer.h" "Data/VarChar.cpp" "Data/VarChar.h" "Data/Basic/ContextManager.cpp" "Data/Basic/ContextManager.h" "Data/Basic/BasicType.h" "Data/Basic/BasicType.cpp" "Stream/ElfStream.h" "Stream/ElfStream.cpp" "Container/IsoContainer.cpp" "Container/IsoContainer.h"   "Data/TableRef.h" "Data/TableRef.cpp" "Data/Storage/BinaryBlock.h" "Data/Storage/BinaryBlock.cpp" "Data/BasicTypeCreator.h" "Data/BasicTypeCreator.cpp")

target_include_directories (bis INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries (bis PUBLIC mulert)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET bis PROPERTY CXX_STANDARD 20)
endif()
