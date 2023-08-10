# 对象模型

本系统的对象模型设计。

## 对象模型概述

本系统（MULE）为将二进制数据文本化/反文本化而设计。
将能够进行二进制读写的基本单元视作对象“Object”，而具有一定数据结构的对象则是结构体“Structure”，一个对象的反复出现，视作数组“Array”。

本系统，认定结构体以数组形式在对象二进制流中组织，每个数组对应一个表“Table”。其中含有紧凑存储的Structure。因此，本系统要求所有的对齐用填充都显式声明。

## 数据流简述

本系统允许复合的结构。用于将二进制流解释为MultiValue或将MultiValue解释为二进制流的对象，称作Type，所有进行解析的类，都从Type继承。

每次，Type解析出一个MultiValue，或者需要获取MultiValue以写入时，都会调用数据处理器（DataHandler）。
数据处理器以类似事件的方式处理数据的读取和写入，每个对象开始读取/写入的前后都产生一次事件，每个单元（字段）读取后，写入前也分别产生事件（以及传输数据）。

```
         (Field/Structure/...)
+--------+ Type*              +--------------+ DataHandler*  +-----------+
| Binary |------------------->| MultiValue   |-------------->| Text/     |
| Stream |<-------------------|              |<--------------| Lua Table |
+--------+              Type* +--------------+  DataHandler* +-----------+

Table的读取控制流如下：
Table::Read()+---->DataHandler::BeforeSheetRead()
             +---->DataHandler::OnRealmEnter()
             +---->Structure::Read()*见下
             +---->DataHandler::OnRealmExit()
             +---->DataHandler::OnRealmEnter()
             +---->Structure::Read()*见下
			 +...
			 +---->DataHandler::AfterSheetRead()

Structure::Read()+---->Field::Read()+----->DataHandler::OnRealmEnter()
                 |                  +----->Type::Read()+----->OnDataRead()*
                 |                  +----->DataHandler::OnRealmExit()
		         +---->Field::Read()...
*并不一定，Type可能是其他Struct等，可能调用更多OnRealmEnter/Exit()，但最终应当调用OnDataRead()

写入控制流：和上面类似
```

## 扩展性和自定义
本系统，使用以下几个基类。通过适当地继承和注册他们，即可实现新的行为。

### 类型
类型，是自我完成的，自由对数据处理器和流进行操作的组件。Type 类是抽象类，用于定义其接口。

文件 mule/Data/Basic/Type.h。

继承并实现此类，并在 mule::Data::TypeManager 注册即可使其被本系统发现。

如果Type中引用了另一个Type，则按约定，在调用那个Type的读写之前，需要首先调用OnRealmEnter，并在之后调用OnRealmExit。这不一定：如果
只是另一个Type的简单包装则不必调用（即，意图对Handler隐藏实现细节，并伪装出只有一个Realm的时候）。

举例而言，Reference保有一个内部Type，其在读取当前uint32_t后，Seek到相应位置再读取内部Type。

每个类型都必须实现GetTypeName来向系统反馈自己的类型名。

除了实现常规的Read和Write外，一些特殊类型还应重载其他函数来保证程序正常处理：

对于不定长类型，须在Size()中返回-1，并重载GetLastSize()和EvalSize()。前者返回上一次Read时读取的字节数，后者返回若要写入指定的对象，将占据的字节数。

对于复合类型，须重载IsComposite()，并始终返回0。

### 数据处理器
数据处理器是对数据进行处理的部分，是二进制对象和MultiValue对象转换的介面。

在文件 mule/Data/Type.h 中定义，是 Type 的内部类。

在读取二进制流时，数据处理器需要在OnRead中处理读取到的数据，例如写入文件流或标准输出流中，也可能是将其转换为 Lua 对象。

类似地，在写入二进制流时，Type会通过OnWrite调用数据处理器，数据处理器需要从 Lua 对象，文件流或标准输入流获取数据，正确地处理其包装并将需要写入的实际数据返回给调用方（即字段处理器）。

有特殊的事件：OnRealmEnter和OnRealmExit：Type可能在适当时机（主要是复合类型、字段等）调用这两个函数，用于表示进入了新的领域。可以依照此处理类型结构。

已经实现了Mappifier，可转换为MultiValue。即使不实现数据处理器，直接用Mappifier获取std::map后对此进行处理也是可行的。

#### 类型创建器
类型创建器处理类型创建请求并返回配置好的类型对象。

文件 mule/Data/TypeCreator.h

类型创建器需要实现 Basic::Field *DoCreateType(std::string &info) 来处理字段的创建。如果对应的创建信息不可处理，请返回 nullptr 来移交控制流到下一个创建器。

#### 类型管理器
用于全局管理类型对象的注册信息，提供已注册的类型对象。并托管一个类型创建器责任链，以处理类型的创建请求。

文件 mule/Data/TypeManager.h

在系统初始化时，应当将需要的对象注册到本管理器，以使对应对象对于系统可见。否则即使实现了对象也无法进行处理。

并且，如果对象需要在运行时创建，其对应的类型创建器必须注册到类型管理器。

通过 ObjectManager::Register() 可以在本管理器注册。推荐在注册后由本管理器管理其生命周期，因此最好立即放弃实例的指针。

使用 ObjectManager::Release() 可以释放对象，这同时会删除对象实莉。

欸？没有了？
