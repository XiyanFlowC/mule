# MULE

~~MULE is a useless, limited editor.~~

## BUILD
```bash
mkdir out
cd out
cmake ..
make all
```

## 用法
```bash
mule <path-to-work-area> <path-to-target-file> <action>
```

* path-to-work-area：到工作区的路径。
* path-to-target-file：到目标文件的路径。
* action：动作，若为 interact 则进入交互模式。

## 工作区
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
  <结构1>
    <字段1 type="string*" />
    <字段2 type="int32" />
    <字段3 type="int32" />
  </结构1>
  <结构2>
    <字段1 type="string*" />
    <字段2 type="string*" />
  </结构2>
  <!-- 其他结构 -->
</def>
```

（尚未完成）
也可以使用INI定义文件，每个文件，具有如下形式：
```ini
[结构1]
字段1=string*
字段2=int32
字段3=uint32
[结构2]

# ...
```
### 脚本目录
命令中的 action 将调用此目录下相应的 lua 脚本。

如 mule . ../foo.iso export 将执行 ./scr/export.lua。lua 中，调用 require() 也将在此目录下搜索（C 模块被禁用）。

### 资源目录
代码中，向 ResourceManager 请求的资源文件从这里开始打开。（目前无效）

### 数据目录
从目标文件中抽取的对象将放置在这里。（目前无效）

默认情况下，将按数据的 CRC32 进行存放。
