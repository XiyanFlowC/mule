# 插件

插件编写约定：

插件须实现一函数，该函数返回结构体的指针：
```cpp
struct PluginDescription
{
	const char *name; // 插件名
	const char *author; // 插件作者
	const char *licence; // 许可类型
	const int majorVer; // 版本
	const int minorVer; // 次要版本
	const char *description; // 插件描述
	mule::Data::TypeCreator *(*GetCreators)(); // 获取类型创建者链
	mule::Data::Basic::Type::DataHandler *(*GetHandler)(const char *name); // 获取指定名称的数据处理器
	xybase::Stream *(*GetStream)(const char *name); // 获取指定名称的流
	xybase::Stream *(*ApplyStream)(const char *name, xybase::Stream *infraStream); // 对指定的流应用指定的转义流解释
	xybase::FileContainer *(*GetFileContainer)(const char *name, xybase::Stream *infraStream); // 对指定的流以指定的容器流解释
};

PluginDescription* GetDescription();
```

通过指定上述字段，向调用者提供功能。

## 插件目录
插件须保存在工作目录下，并以plugin.lst文件详列需要加载的插件。
