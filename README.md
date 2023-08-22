# MULE

~~MULE is a useless, limited editor.~~

## 构建/BUILD
```bash
mkdir out
cd out
cmake ..
make all
```

请注意：你的编译器应支持C++20标准，CMake版本不应低于3.12。

当前，确认可用的编译器为g++（GCC）和cl（MSVC）。

Be noticed: your compiler should support C++20, and the version of your cmake should grather than 3.12.
The comments were written in Chinese (if I have wrote).

## 用法/Usage
```bash
mule <path-to-work-area> <path-to-target-file> <action>
```

* path-to-work-area：到工作区的路径。
* path-to-target-file：到目标文件的路径。
* action：动作，若为 interact 则进入交互模式。If it is set to 'interact', the programe will run in the interaction mode.

## 工作区/Work Area
工作区需要具备以下条件：
* 具有脚本子目录 scr
* 具有数据子目录 dat
* 具有资源子目录 res
* 具有表格子目录 sht
* 具有定义子目录 def

### 定义目录
其中保存各XML定义文件。每个文件，具有如下形式：
```xml
<def>
  <struct name="结构1" onread="foo" onwrite="foo">
    <field name="字段1" type="string*" />
    <field name="字段2" type="int32" />
    <field name="字段3" type="int32">
        <!-- 如果有元信息，则在这里登记，不同的类型支持不同的元信息 -->
        <cache>字段</cache>
        <!-- 即使登记了不受支持的元信息，也不会发生异常 -->
        <invalid>Foo</invalid>
    </field>
  </struct>
  <struct name="结构2">
    <field name="字段1" type="string*" />
    <field name="字段2" type="string*" />
  </struct>
  <!-- 其他结构 -->
  <script>
    function foo()
      print('foo() called');
    end
  </script>
</def>
```

### 脚本目录
命令中的 action 将调用此目录下相应的 lua 脚本。

如 mule . ../foo.iso export 将执行 ./scr/export.lua。lua 中，调用 require() 也将在此目录下搜索（C 模块被禁用）。

### 资源目录
代码中，向 ResourceManager 请求的资源文件从这里开始打开。（目前无效）

### 数据目录
从目标文件中抽取的对象将放置在这里。（目前无效）

默认情况下，将按数据的 CRC32 进行存放。
