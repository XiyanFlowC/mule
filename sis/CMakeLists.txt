
add_library (sis   "Xml/XmlHandler.h" "Xml/XmlHandler.cpp" "Xml/XmlGenerator.h" "Xml/XmlParser.h"  "Xml/MvXmlNode.h" "Xml/MvXmlNode.cpp" "Xml/XmlNode.cpp" "Xml/XmlNode.h" "Xml/XmlParser.cpp" "Cpp/Environment.cpp" "Cpp/Environment.h" "Cpp/BisEnv.h" "Cpp/BisEnv.cpp" "Cpp/CStyleInitHandler.cpp" "Cpp/CStyleInitHandler.h" "Csv/CsvHandler.h" "Csv/CsvHandler.cpp" "Cpp/Context.cpp" "Cpp/Context.h")

target_include_directories (sis INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if (USING_LUA)
add_subdirectory(Lua)
endif()

if (USING_RUBY)
add_subdirectory(Ruby)
endif()

target_link_libraries(sis PUBLIC bis ${CMAKE_DL_LIBS})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET sis PROPERTY CXX_STANDARD 20)
endif()
