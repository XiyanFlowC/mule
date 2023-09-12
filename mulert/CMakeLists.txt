set (MULERT_SOURCES "Data/Basic/Type.cpp" "Data/Basic/Type.h" "Data/Basic/MultiValue.cpp" "Data/Basic/MultiValue.h" "Data/Space/Fragment.cpp" "Data/Space/Fragment.h" "Data/Space/FragmentManager.cpp" "Data/Space/FragmentManager.h" "Data/Storage/BinaryData.cpp" "Data/Storage/BinaryData.h" "Data/Storage/DataManager.cpp" "Data/Storage/DataManager.h"   "Data/TypeCreator.cpp" "Data/TypeCreator.h" "Data/TypeManager.cpp" "Data/TypeManager.h" "mulert_api.h")

add_library (mulert SHARED ${MULERT_SOURCES})
add_library (mulert_sta ${MULERT_SOURCES})

target_compile_definitions (mulert PRIVATE MULERT_EXPORTS_DLL)
target_compile_definitions (mulert_sta PUBLIC MULERT_STATIC)

target_include_directories (mulert INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (mulert_sta INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries (mulert PUBLIC xybase_dyn)
target_link_libraries (mulert_sta PUBLIC xybase)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET mulert PROPERTY CXX_STANDARD 20)
  set_property(TARGET mulert_sta PROPERTY CXX_STANDARD 20)
endif()
