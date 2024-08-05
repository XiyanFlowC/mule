set (XY_BASE_SOURCES "BinaryStream.cpp" "Stream.cpp" "Stream.h" "StreamBasic.cpp" "xystring.cpp" "xyutils.cpp" "Exception/InvalidOperationException.cpp" "Exception/InvalidParameterException.cpp" "Exception/IOException.cpp" "Exception/NotImplementedException.cpp" "Exception/OutOfRangeException.cpp" "Exception/RuntimeException.cpp" "Exception/Exception.cpp" "TextStream.cpp" "FileContainer.cpp" "Xml/XmlNode.cpp" "FileContainerBasic.cpp" "HostFsMapper.cpp" "Fragment/Fragment.cpp" "Fragment/FragmentManager.cpp"  "MemoryStream.cpp")
set (XY_BASE_HEADERS "BinaryStream.h" "FileContainer.h" "Singleton.h" "StreamBasic.h" "StringBuilder.h" "xystring.h" "xyutils.h" "Exception/InvalidOperationException.h" "Exception/InvalidParameterException.h" "Exception/IOException.h" "Exception/NotImplementedException.h" "Exception/OutOfRangeException.h" "Exception/RuntimeException.h" "Exception/Exception.h"  "TextStream.h"  "Xml/XmlParser.h" "Xml/XmlNode.h" "FileContainerBasic.h" "HostFsMapper.h" "Fragment/Fragment.h" "Fragment/FragmentManager.h" "Event.h" "MemoryStream.h")
add_library (xybase ${XY_BASE_SOURCES} ${XY_BASE_HEADERS})
add_library (xybase_dyn SHARED ${XY_BASE_SOURCES} ${XY_BASE_HEADERS})

target_compile_definitions (xybase_dyn PUBLIC XY_DYNAMIC_LIBRARY)
target_compile_definitions (xybase_dyn PRIVATE XY_EXPORT_DLL)

target_include_directories (xybase INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (xybase_dyn INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

include (GNUInstallDirs)
install (TARGETS xybase DESTINATION ${CMAKE_INSTALL_LIBDIR}/xybase)
install (TARGETS xybase_dyn DESTINATION ${CMAKE_INSTALL_LIBDIR}/xybase)
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xybase FILES_MATCHING PATTERN "*.h")

if (MSVC)
  target_compile_options(xybase PRIVATE "/utf-8")
  target_compile_options(xybase_dyn PRIVATE "/utf-8")
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET xybase PROPERTY CXX_STANDARD 20)
  set_property(TARGET xybase_dyn PROPERTY CXX_STANDARD 20)
endif()
