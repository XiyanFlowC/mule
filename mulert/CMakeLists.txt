set (MULERT_SOURCES "Data/Basic/Type.cpp" "Data/Basic/MultiValue.cpp" "Data/Space/Fragment.cpp" "Data/Space/FragmentManager.cpp" "Storage/BinaryData.cpp" "Storage/DataManager.cpp"  "Data/TypeCreator.cpp" "Data/TypeManager.cpp" "VirtualFileSystem.cpp" "Mule.cpp" "TranscripterManager.cpp" "Logger.cpp"   "Configuration.cpp" "Storage/ResourceManager.cpp" "Data/Array.cpp" "Data/Reference.cpp" "Data/Structure.cpp" "Data/Basic/BasicType.cpp" "Data/Basic/ContextManager.cpp"  "Data/ReferenceManager.cpp" "Data/Sheet.cpp" "SheetManager.cpp" "SheetReference.cpp")
set (MULERT_HEADERS  "Data/Basic/Type.h"  "Data/Basic/MultiValue.h"  "Data/Space/Fragment.h"  "Data/Space/FragmentManager.h" "Storage/BinaryData.h" "Storage/DataManager.h"  "Data/TypeCreator.h" "Data/TypeManager.h" "mulert_api.h" "VirtualFileSystem.h" "Mule.h" "TranscripterManager.h"   "Logger.h" "Configuration.h" "Storage/ResourceManager.h" "Data/Array.h" "Data/Reference.h" "Data/Structure.h" "Data/Basic/BasicType.h" "Data/Basic/ContextManager.h"  "Data/ReferenceManager.h" "Data/Sheet.h" "SheetManager.h" "SheetReference.h")

add_library (mulert SHARED ${MULERT_SOURCES} ${MULERT_HEADERS})
add_library (mulert_sta ${MULERT_SOURCES} ${MULERT_HEADERS})

target_compile_definitions (mulert PRIVATE MULERT_EXPORTS_DLL)
target_compile_definitions (mulert_sta PUBLIC MULERT_STATIC)

target_include_directories (mulert INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (mulert_sta INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries (mulert PUBLIC xybase_dyn ${CMAKE_DL_LIBS})
target_link_libraries (mulert_sta PUBLIC xybase ${CMAKE_DL_LIBS})

include (GNUInstallDirs)
install (TARGETS mulert DESTINATION ${CMAKE_INSTALL_LIBDIR}/mule)
install (TARGETS mulert_sta DESTINATION ${CMAKE_INSTALL_LIBDIR}/mule)
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mule FILES_MATCHING PATTERN "*.h")

if (MSVC)
  target_compile_options(mulert PRIVATE "/utf-8")
  target_compile_options(mulert_sta PRIVATE "/utf-8")
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET mulert PROPERTY CXX_STANDARD 20)
  set_property(TARGET mulert_sta PROPERTY CXX_STANDARD 20)
else()
  message (FATAL "CMake 3.12 is required!")
endif()
