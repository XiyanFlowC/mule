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

## 用法/Usage
```bash
mule <path-to-work-area> <path-to-target-file> <action> [options]
```

* path-to-work-area：到工作区的路径。
* path-to-target-file：到目标文件的路径。
* action：动作，若为 interact 则进入交互模式。
* options:
  * -l: less log.
  * -d<key>=<value> define config value. 设置配置变量。可接受的配置变量参见文档。

## 工作区/Work Area
工作区是用于存储定义信息/脚本以及导出结果/导入用数据的目录。

### 资源目录
资源目录在资源管理器ResourceManager中使用，由mule.resource.basedir定义。程序的其他组件通过ResourceManager访问资源。

mule.cpp中，默认设定mule.resource.basedir为${mule.basedir}resources/。

### 定义目录
定义目录，是资源目录下的definition/。（${mule.resource.basedir}definition/）

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
由mule.script.basedir定义，mule.cpp中默认设定为#{mule.resource.basedir}scripts/。

命令中的 action 将调用此目录下相应的 lua 脚本。

如 mule . ../foo.iso export 将执行 ./scr/export.lua。lua 中，调用 require() 也将在此目录下搜索（C 模块被禁用）。

#### 脚本中注册的C函数
（略）

### 数据目录
由mule.data.basedir定义，mule.cpp中默认设定为${mule.basedir}data/。

从目标文件中抽取的二进制对象，以及程序所需要的资源文件将放置在这里。

其中00/保留给Mule系统使用。01/-09/用于二进制对象保存。

要在其中保存自己定义的数据文件，建议使用0A/开始的编号。

### 表格目录
由mule.sheet.basedir定义，mule.cpp中默认设定为${mule.basedir}sheets/。

从目标文件中抽取的表格将被保存在这里。导入时，也从这里获取表格数据。
