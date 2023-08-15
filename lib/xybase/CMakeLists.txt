
add_library (xybase "BinaryStream.cpp" "BinaryStream.h" "FileContainer.h" "Singleton.h" "Stream.cpp" "Stream.h" "StreamBasic.cpp" "StreamBasic.h" "StringBuilder.h" "xystring.cpp" "xystring.h" "xyutils.cpp" "xyutils.h" "Exception/InvalidOperationException.cpp" "Exception/InvalidOperationException.h" "Exception/InvalidParameterException.cpp" "Exception/InvalidParameterException.h" "Exception/IOException.h" "Exception/IOException.cpp" "Exception/NotImplementedException.h" "Exception/NotImplementedException.cpp" "Exception/OutOfRangeException.cpp" "Exception/OutOfRangeException.h" "Exception/RuntimeException.cpp" "Exception/RuntimeException.h" "Exception/Exception.cpp" "Exception/Exception.h")

target_include_directories (xybase INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET xybase PROPERTY CXX_STANDARD 20)
endif()
