project (mulert VERSION 1.11)

string (TIMESTAMP COMPILE_TIME %Y/%m/%d-%H:%M:%S)
set (BUILD_TIME ${COMPILE_TIME})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/MuleRtVersion.h.in ${CMAKE_CURRENT_SOURCE_DIR}/MuleRtVersion.h)

set (MULERT_SOURCES "Data/Basic/Type.cpp" "Data/Basic/MultiValue.cpp" "Storage/BinaryData.cpp" "Storage/DataManager.cpp"  "Data/TypeCreator.cpp" "Data/TypeManager.cpp" "VirtualFileSystem.cpp" "Mule.cpp" "TranscripterManager.cpp" "Logger.cpp"   "Configuration.cpp" "Storage/ResourceManager.cpp" "Data/Array.cpp" "Data/Reference.cpp" "Data/Structure.cpp" "Data/Basic/BasicType.cpp" "Data/Basic/ContextManager.cpp"   "Data/Sheet.cpp" "SheetManager.cpp" "SheetReference.cpp" "Data/SmartReference.cpp")
set (MULERT_HEADERS  "Data/Basic/Type.h"  "Data/Basic/MultiValue.h" "Storage/BinaryData.h" "Storage/DataManager.h"  "Data/TypeCreator.h" "Data/TypeManager.h" "mulert_api.h" "VirtualFileSystem.h" "Mule.h" "TranscripterManager.h"   "Logger.h" "Configuration.h" "Storage/ResourceManager.h" "Data/Array.h" "Data/Reference.h" "Data/Structure.h" "Data/Basic/BasicType.h" "Data/Basic/ContextManager.h"   "Data/Sheet.h" "SheetManager.h" "SheetReference.h" "Data/SmartReference.h" "MuleRtVersion.h")

add_library (mulert SHARED ${MULERT_SOURCES} ${MULERT_HEADERS})
add_library (mulert_sta ${MULERT_SOURCES} ${MULERT_HEADERS})

target_compile_definitions (mulert PRIVATE MULERT_EXPORTS_DLL)
target_compile_definitions (mulert_sta PUBLIC MULERT_STATIC)

target_include_directories (mulert INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (mulert_sta INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries (mulert PUBLIC xybase_dyn ${CMAKE_DL_LIBS})
target_link_libraries (mulert_sta PUBLIC xybase ${CMAKE_DL_LIBS})

include (GNUInstallDirs)
install (TARGETS mulert DESTINATION ${CMAKE_INSTALL_LIBDIR})
install (TARGETS mulert_sta DESTINATION ${CMAKE_INSTALL_LIBDIR})
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ 
         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mule 
         FILES_MATCHING PATTERN "*.h"
         PATTERN "CMakeFiles" EXCLUDE)

# Platform-specific compiler options
if (MSVC)
  target_compile_options(mulert PRIVATE "/utf-8")
  target_compile_options(mulert_sta PRIVATE "/utf-8")
elseif(UNIX)
  target_compile_options(mulert PRIVATE "-fPIC")
endif()

# Ensure C++ standard
set_property(TARGET mulert PROPERTY CXX_STANDARD 20)
set_property(TARGET mulert_sta PROPERTY CXX_STANDARD 20)
set_property(TARGET mulert PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET mulert_sta PROPERTY CXX_STANDARD_REQUIRED ON)
