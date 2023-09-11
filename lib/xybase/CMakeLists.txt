set (XY_BASE_SOURCES "BinaryStream.cpp" "BinaryStream.h" "FileContainer.h" "Singleton.h" "Stream.cpp" "Stream.h" "StreamBasic.cpp" "StreamBasic.h" "StringBuilder.h" "xystring.cpp" "xystring.h" "xyutils.cpp" "xyutils.h" "Exception/InvalidOperationException.cpp" "Exception/InvalidOperationException.h" "Exception/InvalidParameterException.cpp" "Exception/InvalidParameterException.h" "Exception/IOException.h" "Exception/IOException.cpp" "Exception/NotImplementedException.h" "Exception/NotImplementedException.cpp" "Exception/OutOfRangeException.cpp" "Exception/OutOfRangeException.h" "Exception/RuntimeException.cpp" "Exception/RuntimeException.h" "Exception/Exception.cpp" "Exception/Exception.h"  "TextStream.cpp" "TextStream.h" "FileContainer.cpp")
add_library (xybase ${XY_BASE_SOURCES})

add_library (xybase_dyn SHARED ${XY_BASE_SOURCES})
target_compile_definitions (xybase_dyn PUBLIC XY_DYNAMIC_LIBRARY)
target_compile_definitions (xybase_dyn PRIVATE XY_EXPORT_DLL)

target_include_directories (xybase INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (xybase_dyn INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET xybase PROPERTY CXX_STANDARD 20)
  set_property(TARGET xybase_dyn PROPERTY CXX_STANDARD 20)
endif()
