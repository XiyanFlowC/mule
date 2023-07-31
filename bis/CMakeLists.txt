
# TODO: 若在大端系统中，请启用此定义
# add_definitions(-DBIG_ENDIAN_SYSTEM)

add_library (bis "Data/Basic/Object.h" "Data/ObjectCreator.cpp" "Data/ObjectCreator.h" "Data/Basic/MultiValue.h" "Exception/Exception.h" "Exception/Exception.cpp" "Data/Basic/MultiValue.cpp" "Exception/InvalidOperationException.h" "Exception/InvalidOperationException.cpp" "xybase/Stream.h" "Data/BinaryStream.h" "Data/BinaryStream.cpp" "Data/Basic/Object.cpp" "Exception/RestrictionViolationException.h" "Exception/RestrictionViolationException.cpp" "Data/Basic/Integer.cpp" "Exception/InvalidParameterException.h" "Exception/InvalidParameterException.cpp" "xybase/Singleton.h"  "Data/Basic/Structure.h" "Data/Basic/Structure.cpp"    "xybase/StringBuilder.h"  "Data/ShiftBinaryStream.h" "Data/ShiftBinaryStream.cpp" "Data/ObjectManager.h" "Data/ObjectManager.cpp"   "Data/Basic/String.cpp" "Data/Basic/String.h" "Data/Space/Fragment.h" "Data/Space/Fragment.cpp" "Data/Space/FragmentManager.h" "Data/Space/FragmentManager.cpp" "Data/Referrence.h"        "Data/Basic/Float.cpp" "Data/Basic/Float.h" "Data/Basic/Double.cpp" "Data/Basic/Double.h" "Data/Referrence.cpp" "Data/Table.cpp" "Data/Table.h" "Data/Mappifier.cpp" "Data/Mappifier.h" "Exception/NotImplementedException.cpp" "Exception/NotImplementedException.h" "xybase/xyutils.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET bis PROPERTY CXX_STANDARD 20)
endif()
