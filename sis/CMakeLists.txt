
add_library (sis "Cpp/StructureBuilder.h" "Cpp/StructureBuilder.cpp" "Xml/XmlHandler.h" "Xml/XmlHandler.cpp" "Xml/XmlGenerator.h" "Xml/XmlParser.h"  "Xml/MvXmlNode.h" "Xml/MvXmlNode.cpp" "Xml/XmlNode.cpp" "Xml/XmlNode.h" "Xml/XmlParser.cpp")

if (USING_LUA)
add_subdirectory(Lua)
endif()

if (USING_RUBY)
add_subdirectory(Ruby)
endif()

target_link_libraries(sis PUBLIC bis)
target_include_directories(sis PUBLIC "${PROJECT_SOURCE_DIR}/bis")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET sis PROPERTY CXX_STANDARD 20)
endif()
