
# TODO: 若在大端系统中，请启用此定义
# add_definitions(-DBIG_ENDIAN_SYSTEM)

add_library (bis "Data/Basic/Type.h" "Data/TypeCreator.cpp" "Data/TypeCreator.h" "Data/Basic/MultiValue.h" "Data/Basic/MultiValue.cpp" "Data/Basic/Type.cpp" "Data/Basic/Integer.cpp" "Data/TypeManager.h" "Data/TypeManager.cpp"   "Data/Basic/String.cpp" "Data/Basic/String.h" "Data/Space/Fragment.h" "Data/Space/Fragment.cpp" "Data/Space/FragmentManager.h" "Data/Space/FragmentManager.cpp" "Data/Referrence.h"        "Data/Basic/Float.cpp" "Data/Basic/Float.h" "Data/Basic/Double.cpp" "Data/Basic/Double.h" "Data/Referrence.cpp" "Data/Table.cpp" "Data/Table.h" "Data/Mappifier.cpp" "Data/Mappifier.h" "Data/Array.h" "Data/Array.cpp" "Data/Structure.cpp" "Data/Structure.h" "BasicContainer.cpp" "BasicContainer.h" "Data/VarChar.cpp" "Data/VarChar.h")

target_include_directories (bis INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries (bis PUBLIC xybase)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET bis PROPERTY CXX_STANDARD 20)
endif()
