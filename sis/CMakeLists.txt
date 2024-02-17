
add_library (sis   "Xml/XmlHandler.h" "Xml/XmlHandler.cpp" "Xml/XmlGenerator.h"   "Xml/MvXmlNode.h" "Xml/MvXmlNode.cpp"      "Cpp/BisEnv.h" "Cpp/BisEnv.cpp" "Cpp/CStyleInitHandler.cpp" "Cpp/CStyleInitHandler.h" "Csv/CsvHandler.h" "Csv/CsvHandler.cpp"   "Xml/XmlDefinition.h" "Xml/XmlDefinition.cpp" "Cpp/Mappifier.cpp" "Cpp/Mappifier.h" "Csv/CsvFileHandler.h" "Csv/CsvFileHandler.cpp")

target_include_directories (sis INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if (USING_LUA)
target_link_libraries(sis PUBLIC lua)
add_subdirectory(Lua)
endif()

if (USING_RUBY)
add_subdirectory(Ruby)
endif()

target_link_libraries(sis PUBLIC bis)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET sis PROPERTY CXX_STANDARD 20)
endif()
