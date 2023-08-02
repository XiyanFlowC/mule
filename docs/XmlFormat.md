# XML 表现形式

XML形式由一定的结构组成。相比于标准的XML，此表现形式不允许XML声明（<? ?>），且文本节点必须是叶节点。

也就是说
```xml
<foo>
text text <bar> aaa </bar> text.
</foo>
```
是不允许的，除非bar可以被注册的回调函数处理，并返回以文本串。

如果无法找到bar的回调函数，bar中的全部数据都将被丢弃。

## XML 解析器
XML 解析器属于 sis 的一部分。位于命名空间 mule::Xml 中。文件为 sis/Xml/XmlProcessor.h。

其可以通过 RegisterTagCallback 注册回调函数。回调函数接受一个 MultiValue 以对其中的值进行处理，并返回以 std::string。回调函数理应于 Parse 被调用前注册。

调用 Parse 来获取 MultiValue 模型。
