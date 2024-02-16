# MULE 运行时

## Mule.cpp/.h

是mule的主要文件。插件管理等功能通过此文件支持。未确定功能的部分也实现在这里。

## SheetManager.cpp/.h

表格管理器。通过在此处注册表（mule::Data::Sheet），可以对特定流（xybase::Stream）下所属的表进行管理。
