# 对象模型

本系统的对象模型设计。

## 对象模型概述

本系统（MULE）为将二进制数据文本化/反文本化而设计。
将能够进行二进制读写的基本单元视作对象“Object”，而具有一定数据结构的对象则是结构体“Structure”。
结构体中的数据，视作字段“Field”。

本系统，认定结构体以数组形式在对象二进制流中组织，每个数组对应一个表“Table”。其中含有紧凑存储的Structure。因此，本系统要求所有的填充都显式声明。

## 数据流简述

因为认定所有表内结构体结构一致，因此，本系统规定，Table只在开始读取之前，将结构体数据发送一次到数据处理器（DataHandler），之后只按照此结构不断向数据处理器发送读取到的数据。

同样，写入时，只在开始写入之前，将结构体数据发送一次到数据处理器，之后不断从数据处理器索取数据来写入。

数据处理器以类似事件的方式处理数据的读取和写入，每个表、行（结构体）开始读取/写入的前后都产生一次事件，每个单元（字段）读取后，写入前也分别产生事件（以及传输数据）。

```
         (Field/Structure/...)
+--------+ Object             +--------------+ DataHandler   +-----------+
| Binary |------------------->| MultiValue   |-------------->| Text/     |
| Stream |<-------------------|              |<--------------| Lua Table |
+--------+             Object +--------------+   DataHandler +-----------+

读取控制流：
Table::Read()+---->DataHandler::BeforeSheetRead()
             +---->Structure::Read()*见下
             +---->Structure::Read()*见下
			 +...
			 +---->DataHandler::AfterSheetRead()

Structure::Read()+---->DataHandler::BeforeRecordRead()
                 +---->Field::Read()+----->BinaryStream::Read()
                 |                  +----->DataHandler::OnCellRead()
                 |                  +----->Field(next)::Read()+---->BinaryStream::Read() // Field 设计为链式读取
		         |                                            +---->DataHandler::OnCellRead()
        	     |                                            +---->Field(next)::Read()+...
		         +---->DataHandler::AfterSheetRead/Write()
```

## 扩展性和自定义
本系统，通过预留数个亲类的方式，为修改留下可能性。

### 数据处理器
数据处理器是对数据进行处理的部分，是二进制对象和其他对象转换的界面。

文件 mule/Data/Basic/DataHandler.h，是为数据处理器，可以自己定义的部分有：

- OnSheetReadingStarted() : void
- OnSheetWritingStarted() : void
- OnSheetReadingEnded() : void
- OnSheetWritingEnded() : void
- OnRecordReadingStarted() : void
- OnRecordWritingStarted() : void
- OnRecordReadingEnded() : void
- OnRecordWritingEnded() : void
- OnCellRead(const MultiValue &) : void
- OnCellWrite() : MultiValue

按顺序，是一个表/结构体开始/结束读/写时的事件。最后则是一个单元（字段）读/写前/后的事件。可以通过这些事件来追踪读写进度。
数据处理器需要在OnCellRead中处理读取到的数据，例如写入文件流或标准输出流中，也可能是将其转换为 Lua 对象。
类似地，在写入时，数据处理器需要从 Lua 对象，文件流或标准输入流获取数据，正确地处理其包装并将需要写入的实际数据返回给调用方（即字段处理器）。

### 对象
对象，是自我完成的，自由对数据处理器和流进行操作的组件。Object 类是纯虚类，用于定义其接口。

文件 mule/Data/Basic/Object.h。

继承并实现此类，并在 mule::Data::ObjectManager 注册即可使其被本系统发现。

#### 字段
Field 是对象的子类。大部分时候，应该继承它来实现二进制流读写处理的操作。

文件 mule/Data/Basic/Field.h。

字段需要实现以下方法：DoRead() 和 DoWrite()。

MultiValue DoRead(FieldReadWriteContext &context) 用于从二进制流读取，读取所得的结果直接通过 MultiValue 返回给公开的内定方法处理。

int DoWrite(FieldReadWriteContext &context, const MultiValue &value) 用于将所给 value 写入二进制流。
系统约定此函数返回值若不为 0，则读取链会中断并报告错误（指定的错误码，即返回值）。
系统并不解读错误码或对此有特定处理，仅仅只是中断读写，并输出错误代码，因此这将导致不可恢复的错误。请注意这一点，如果错误是可以恢复的，请在 Field 内部自行处理。

字段还需实现 size_t FieldSize() 函数来报告其所占的空间。通过大小，Field 便能够减少流读写的次数，一次性将大量数据读入到内存处理。（预留功能，暂未实现）

##### 字段创建器

字段需要配合字段创建器使用。字段创建器处理字段创建请求并返回配置好的字段对象。

文件 mule/Data/FieldCreator.h

字段创建器需要实现 Basic::Field *DoCreateField(FieldCreatingInfo &info) 来处理字段的创建。如果对应的创建信息不可相应，请返回 nullptr 来移交控制流到下一个创建器。

#### 对象管理器

用于全局管理对象的注册信息，提供已注册的对象。并托管一个字段创建器责任链，以处理字段的创建请求。

文件 mule/Data/ObjectManager.h

在系统初始化时，应当将需要的对象注册到本管理器，以使对应对象对于系统可见。否则即使实现了对象也无法进行处理。

通过 ObjectManager::Register() 可以在本管理器注册。推荐在注册后由本管理器管理其生命周期，因此最好立即放弃实例的指针。

使用 ObjectManager::Release() 可以释放对象，这同时会删除对象实莉。

## 类图
（没写完，摸了）
### 命名空间 mule::Data::Basic

```mermaid
class DataHandler {
	#OnSheetReadingStarted() : void
	#OnSheetWritingStarted() : void
	#OnSheetReadingEnded() : void
	#OnSheetWritingEnded() : void
	#OnRecordReadingStarted() : void
	#OnRecordWritingStarted() : void
	#OnRecordReadingEnded() : void
	#OnRecordWritingEnded() : void
	#OnCellRead(const MultiValue &) : void
	#OnCellWrite() : MultiValue
}
```
