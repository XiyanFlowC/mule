# Lua API

本文档列出当前 Lua 运行环境中可用的函数，包括 Lua 5.4 标准库与 MULE 扩展函数。

## 标准库（Lua 5.4）

运行时加载的模块：`_G`（base）、`package`、`table`、`string`、`math`、`utf8`、`debug`。

- `base`：`assert`、`collectgarbage`、`dofile`、`error`、`getmetatable`、`ipairs`、`load`、`loadfile`、`next`、`pairs`、`pcall`、`print`、`rawequal`、`rawget`、`rawlen`、`rawset`、`select`、`setmetatable`、`tonumber`、`tostring`、`type`、`xpcall`、`warn`。
- `package`：`config`、`cpath`、`loaded`、`path`、`preload`、`searchers`、`searchpath`、`loadlib`。
- `table`：`concat`、`insert`、`move`、`pack`、`remove`、`sort`、`unpack`。
- `string`：`byte`、`char`、`dump`、`find`、`format`、`gmatch`、`gsub`、`len`、`lower`、`match`、`rep`、`reverse`、`sub`、`upper`、`pack`、`packsize`、`unpack`。
- `math`：`abs`、`acos`、`asin`、`atan`、`ceil`、`cos`、`deg`、`exp`、`floor`、`fmod`、`huge`、`log`、`max`、`maxinteger`、`min`、`mininteger`、`modf`、`pi`、`rad`、`random`、`randomseed`、`sin`、`sqrt`、`tan`、`tointeger`、`type`、`ult`。
- `utf8`：`char`、`charpattern`、`codepoint`、`codes`、`len`、`offset`。
- `debug`：`debug`、`gethook`、`getinfo`、`getlocal`、`getmetatable`、`getregistry`、`getupvalue`、`getuservalue`、`sethook`、`setlocal`、`setmetatable`、`setupvalue`、`setuservalue`、`traceback`、`upvalueid`、`upvaluejoin`。

## 基础信息

### 虚拟文件路径

MULE 使用虚拟文件系统（VFS）统一访问目标文件与挂载的容器。路径格式为：

- `root:path`：`root` 为挂载根名，`:` 后为根内路径。
- 目标文件/目录默认根：
  - 当目标为目录时，默认挂载 `target:`，路径如 `target:/foo/bar.bin`。
  - 当目标为文件时，仅自动打开为流句柄 `0`（不自动挂载 `target:` 根）；此时请直接使用句柄 `0` 进行读写。

示例：

```lua
-- 访问目标目录下的文件
local h = open("target:/data.bin", "r")
close(h)
```

### 配置变量（`config`）

这些变量在启动时由程序设置或解析命令行后写入，可通过 `config(name, nil)` 读取：

- `mule.basedir`：工作区根目录（命令行第 1 个参数）。
- `mule.data.basedir`：数据目录，默认 `${mule.basedir}data/`，由 `DataManager` 读写。
- `mule.resource.basedir`：资源目录，默认 `${mule.basedir}resources/`，由 `ResourceManager` 读写。
- `mule.sheet.basedir`：表格目录，默认 `${mule.basedir}sheets/`，由 `SheetManager` 读写。
- `mule.script.basedir`：脚本目录，默认 `${mule.resource.basedir}scripts/`。
- `mule.target`：目标路径（命令行第 2 个参数）。
- `mule.target.type`：目标类型，`file` 或 `directory`。

运行期自动设置（由系统写入，通常只读）：

- `mule.data.sheet.name`：当前正在读写的 Sheet 名称。
- `mule.data.sheet.index`：当前正在处理的 Sheet 元素索引（从 `0` 开始）。
- `mule.data.sheet.structure-simplify-suppression`：当值非 `0` 时，禁止长度为 `1` 的 Sheet 省略数组结构（影响 `Sheet` 读写流程）。

命令行可用 `-d<key>=<value>` 追加/覆盖任意配置项（值会解析 `${var}` 占位）。


示例：

```lua
local basedir = config("mule.basedir", nil)
local targetType = config("mule.target.type", nil)
```

### 目标的隐含条件

- 当 `mule.target.type == "directory"`：目标目录自动挂载为 `target:` 根。
- 当 `mule.target.type == "file"`：目标文件自动以 `rb+` 打开并注册为流句柄 `0`。
- 默认执行顺序：先运行 `config.lua`，再运行 `action` 对应脚本。

## MULE 扩展函数

### MULE 函数错误处理约定
错误处理：多数函数返回 `int`，`0` 表示成功，负值通常表示错误或异常码（具体码来自内部抛出的异常或检查失败）。请参见各函数说明。

特别地，由于未处理的异常导致的失败，和调用前约定预检失败，返回值为 nil，然后第二个返回值为错误码。否则，第二个返回值为 nil。
```lua
local result, ex = someFunction(...)
if ex ~= nil then
  -- 处理错误，ex 是异常错误码
else
  -- 处理成功结果
end
```

但是，部分函数在错误时，只是返回错误码，而不返回 nil。这些函数的调用者需要根据返回值是否为 0 来判断成功与否。

### 流操作（Stream）

流操作函数面向虚拟文件系统与底层 `Stream`，以句柄（`int`）标识打开的流。多数函数返回 `int`，`0` 表示成功，负值通常表示错误或异常码（具体码来自内部抛出的异常或检查失败）。

#### 基本打开/关闭

- `open(path, mode) -> int`
  - 参数：
    - `path`：MULE 内部路径（如 `target:`、挂载根下的路径）。
    - `mode`：打开模式字符串，可组合：`r` 读、`w` 写、`a` 追加、`t` 截断、`x` 独占、`b` 大端。
  - 返回：流句柄（`int`）。失败返回 `-10`。
  - 内部处理：通过虚拟文件系统打开目标并注册到 Lua 环境的流表。

- `close(hndl) -> int`
  - 参数：
    - `hndl`：流句柄。
  - 返回：`0` 成功；异常时返回异常错误码。
  - 内部处理：关闭并释放对应的 `Stream`，从 Lua 环境的流表移除。

#### 读写字节与数组

- `read(hndl, size) -> table`
  - 参数：
    - `hndl`：流句柄。
    - `size`：读取字节数。
  - 返回：1-based 的字节数组（Lua table，元素为 `0..255` 的整数）。
  - 内部处理：按长度读入缓冲区，逐字节转换为数组。

- `write(hndl, data) -> int`
  - 参数：
    - `hndl`：流句柄。
    - `data`：1-based 的字节数组（Lua table，元素为整数）。
  - 返回：`0` 成功。
  - 内部处理：按索引顺序读取数组，拼接为字节序列后写入流。

- `readbyte(hndl) -> int` / `writebyte(hndl, byte) -> int`
  - 参数：
    - `hndl`：流句柄。
    - `byte`：写入字节值（`0..255`）。
  - 返回：`readbyte` 返回读到的字节；`writebyte` 返回 `0`。
  - 内部处理：单字节读写。

#### 读写数值（整数/浮点）

这些函数直接调用底层 `Stream` 的类型读写接口，采用流当前的字节序（由 `open` 的 `b` 标志或流自身决定）。

- 无符号整数：`readu16`/`writeu16`、`readu32`/`writeu32`、`readu64`/`writeu64`
  - 参数：
    - `hndl`：流句柄。
    - `value`：写入值（对应位宽）。
  - 返回：读函数返回数值；写函数返回 `0`。
  - 内部处理：按位宽进行读写，写入时会截断到对应位宽。

- 有符号整数：`readi16`/`writei16`、`readi32`/`writei32`、`readi64`/`writei64`
  - 参数：
    - `hndl`：流句柄。
    - `value`：写入值（对应位宽）。
  - 返回：读函数返回数值；写函数返回 `0`。
  - 内部处理：按位宽进行读写。

- 浮点数：`readfloat`/`writefloat`、`readdouble`/`writedouble`
  - 参数：
    - `hndl`：流句 handles
    - `value`：写入值（`float`/`double`）。
  - 返回：读函数返回数值；写函数返回 `0`。
  - 内部处理：按 IEEE 浮点格式读写。

#### 位置控制

- `tell(hndl) -> int`
  - 参数：
    - `hndl`：流句柄。
  - 返回：当前偏移。
  - 内部处理：调用 `Stream::Tell()`。

- `seek(hndl, offset, seekType) -> int`
  - 参数：
    - `hndl`：流句柄。
    - `offset`：偏移量。
    - `seekType`：寻址方式（对应底层 `Stream::SeekMode`：`0` 起始、`1` 当前位置、`2` 末尾）。
  - 返回：新的偏移。
  - 内部处理：调用 `Stream::Seek()` 后返回 `Tell()`。

#### 文件导入/导出

- `export(path, id) -> int`
  - 参数：
    - `path`：虚拟文件路径（如 `target:` 或挂载根下的路径）。
    - `id`：数据文件编号（`data/0x` 目录中的编号，由 DataManager 管理）。
  - 返回：`0` 成功。
  - 内部处理：读取指定虚拟文件的全部内容并保存为数据文件（`DataManager::SaveData`）。

- `resexport(src, out) -> int`
  - 参数：
    - `src`：虚拟文件路径。
    - `out`：资源路径（资源管理器中的保存路径）。
  - 返回：`0` 成功，异常时返回错误码。
  - 内部处理：读取虚拟文件内容并保存到资源管理器。

- `extract(path, offset, length, id) -> int`
  - 参数：
    - `path`：虚拟文件路径。
    - `offset`：起始偏移。
    - `length`：长度。
    - `id`：数据文件编号。
  - 返回：`0` 成功。
  - 内部处理：从虚拟文件读取区段并保存为数据文件。

- `import(path, id) -> int`
  - 参数：
    - `path`：虚拟文件路径。
    - `id`：数据文件编号。
  - 返回：`0` 成功。
  - 内部处理：从数据文件读取并覆盖写入虚拟文件（会截断原内容）。

- `resimport(dst, in) -> int`
  - 参数：
    - `dst`：虚拟文件路径。
    - `in`：资源路径。
  - 返回：`0` 成功，异常时返回错误码。
  - 内部处理：从资源管理器加载并写入目标虚拟文件。

- `patch(path, offset, length, id) -> int`
  - 参数：
    - `path`：虚拟文件路径。
    - `offset`：起始偏移。
    - `length`：长度（与数据文件实际长度一致时最安全）。
    - `id`：数据文件编号。
  - 返回：`0` 成功。
  - 内部处理：从数据文件读取并写入虚拟文件的指定偏移（不截断）。

#### 使用示例

```lua
-- 抽取目标文件全量到 data 目录编号 1
export("root:path/to/file.bin", 1)

-- 从 data/00/00/00/01.DAT 导入回目标文件
import("root:path/to/file.bin", 1)

-- 抽取一段数据
extract("root:path/to/file.bin", 0x100, 0x200, 2)

-- 覆盖写入一段补丁数据
patch("root:path/to/file.bin", 0x100, 0x200, 2)
```

### 数据目录（DataManager）

`mule.data.basedir` 是数据文件根目录，`export`/`extract`/`import`/`patch` 读写的都是这里的文件。文件路径按 32-bit `id` 拆分为四级目录：

```
${mule.data.basedir}%02X/%02X/%02X/%02X.dat
```

其中 4 个字节分别为 `id >> 24`、`id >> 16`、`id >> 8`、`id` 的十六进制两位表示。示例：

- `id = 0x00000001` → `data/00/00/00/01.dat`
- `id = 0x01000000` → `data/01/00/00/00.dat`

### 资源文件目录（ResourceManager）

`mule.resource.basedir` 是资源目录根路径。`resexport`/`resimport` 使用的 `path` 直接拼接到该根目录：

```
${mule.resource.basedir}<path>
```

示例：`resexport("target:/data.bin", "resources/bin.dump")` 会写入 `${mule.resource.basedir}resources/bin.dump`。

### 容器与虚拟文件系统

该组函数管理虚拟文件系统（VFS）中的挂载根与容器。路径格式为 `root:/path`。

#### 挂载与卸载

- `mount(hndl, type, root) -> int`
  - 参数：
    - `hndl`：流句柄，作为容器的下层流。
    - `type`：容器类型名（由插件或内置容器解释器提供）。
    - `root`：挂载根名（不带 `:`）。
  - 返回：`0` 成功，失败返回 `-10`（流句柄无效）。
  - 内部处理：将指定流通过容器解释器挂载到 VFS 根下。

- `openpack(root, param) -> int`
  - 参数：
    - `root`：挂载根名。
    - `param`：容器打开参数字符串。
  - 返回：`0` 成功。
  - 内部处理：将 `param` 按空格分割（支持双引号包裹带空格参数），传递给插件 `OpenContainer(count, params)`。参数的具体含义由容器插件定义，常见约定为“第一个参数是包类型，其余为该类型的参数列表”，具体请参见对应包解析插件文档。

- `unmount(root) -> int`
  - 参数：
    - `root`：挂载根名。
  - 返回：`0` 成功。
  - 内部处理：从 VFS 移除指定根的挂载。

#### 目录与条目

- `list(root) -> table`
  - 参数：
    - `root`：挂载根名（不带 `:`）。
  - 返回：1-based 的条目数组（Lua table）。
  - 内部处理：调用 VFS 的 `List`，仅列出该根的顶层条目（不支持根内路径）。

- `listroots() -> table`
  - 参数：无。
  - 返回：1-based 的根名数组（Lua table）。
  - 内部处理：调用 VFS 的 `ListRoots`。

- `remove(path) -> int`
  - 参数：
    - `path`：虚拟文件路径（`root:/path`）。
  - 返回：`0` 成功。
  - 内部处理：调用 VFS 删除指定条目。

#### 元数据

- `mdata(path, data) -> table/int`
  - 参数：
    - `path`：虚拟文件路径（`root:/path`）。
    - `data`：
      - `nil`：读取元数据。
      - `table`：写入元数据，字段包含 `path`、`size`、`offset`、`occupied`。
  - 返回：
    - 读取时返回元数据表。
    - 写入时返回 `nil`（或 `0`）。
  - 内部处理：
    - 仅对 `FileContainerBasic` 类型的容器有效；否则返回 `nil`。
    - 读取时返回 `{ path, size, offset, occupied }`。
    - 写入时以表中字段更新条目元数据。

#### 使用示例

```lua
-- 使用 openpack 直接挂载并列出根目录
openpack("pack", "pak ./foo.pak")
local entries = list("pack")
unmount ("pack")
```

```lua
-- 读取与更新元数据
local meta = mdata("pak:/file.bin", nil)
meta.size = meta.size + 16
mdata("pak:/file.bin", meta)
```

### 系统与配置

该组函数负责系统交互、日志与运行时配置。多数函数返回 `int`，`0` 表示成功。

#### 交互与插件

- `confirm(word) -> int`
  - 参数：
    - `word`：期望匹配的字符串。
  - 返回：匹配返回 `0`，不匹配返回 `-1`。
  - 内部处理：从标准输入读取一段文本并与 `word` 比较。

- `pplugin() -> int`
  - 参数：无。
  - 返回：`0`。
  - 内部处理：打印已加载插件列表到日志。

- `lplugin(path) -> int`
  - 参数：
    - `path`：插件路径（不含扩展名，Windows 自动补 `.dll`，Linux 自动补 `.so`，并尝试添加 `lib` 前缀）。
  - 返回：`0`。
  - 内部处理：动态加载插件并注册其描述。

#### 日志

- `log(msg, level) -> int`
  - 参数：
    - `msg`：日志文本。
    - `level`：日志级别，`-1/0/1/2/3/4` 分别表示 `debug/info/note/warn/error/fatal`。`nil` 等价于 `0`。
  - 返回：`0`。
  - 内部处理：按级别调用内部日志器输出。

- `loginit(level, enableColour) -> int`
  - 参数：
    - `level`：日志级别阈值（数值越小越详细）。
    - `enableColour`：是否启用颜色输出。
  - 返回：`0`。
  - 内部处理：初始化日志器。

- `logsetout(filename) -> int`
  - 参数：
    - `filename`：标准输出文件名。
  - 返回：`0`。
  - 内部处理：设置日志标准输出目标。

- `logseterr(filename) -> int`
  - 参数：
    - `filename`：错误输出文件名。
  - 返回：`0`。
  - 内部处理：设置日志错误输出目标。

#### 配置

- `config(name, value) -> any`
  - 参数：
    - `name`：配置键名。
    - `value`：写入值；传 `nil` 表示读取。
  - 返回：
    - 读取时返回配置值（不存在返回 `nil`）。
    - 写入时返回 `0`。
  - 内部处理：读取或写入 `Configuration` 全局变量表。

- `config_erase(name) -> int`
  - 参数：
    - `name`：配置键名。
  - 返回：`0`。
  - 内部处理：从全局配置表移除该键。

#### 结构定义

- `define(typeName, def) -> int`
  - 参数：
    - `typeName`：结构体类型名。
    - `def`：字段定义表（Lua table，元素为 `"field:type"` 字符串）。
  - 返回：`0` 成功；`-10` 参数不是表；`-11` 字段不是字符串；`-12` 类型创建失败。
  - 内部处理：创建 `Structure` 并注册到 `TypeManager`。

#### 使用示例

```lua
loginit(0, true)
log("hello", 0)
logsetout("./stdout.log")
logseterr("./stderr.log")
```

```lua
local v = config("mule.target", nil)
config("my.flag", 1)
config_erase("my.flag")
```

```lua
define("FileEntry", { "offset:int32", "length:int32", "name:string*" })
```

### 结构与表格（Sheet）

该组函数用于管理结构定义、表格注册与导入导出。表格文件位于 `mule.sheet.basedir`，文件名为 `<sheetName>.<handler>`。

#### 结构定义

- `loaddef(xmlText) -> int`
  - 参数：
    - `xmlText`：XML 定义文本内容（不是文件名）。
  - 返回：`0` 成功；解析失败返回负值。
  - 内部处理：解析 XML 并注册类型定义。

- `define(typeName, def) -> int`
  - 参数：
    - `typeName`：结构体类型名。
    - `def`：字段定义表（Lua table，元素为 `"field:type"` 字符串）。
  - 返回：`0` 成功；`-10` 参数不是表；`-11` 字段不是字符串；`-12` 类型创建失败。
  - 内部处理：创建 `Structure` 并注册到 `TypeManager`。

#### 单表读写

- `exportsht(hndl, handler, type, name, offset, length) -> int`
  - 参数：
    - `hndl`：流句柄。
    - `handler`：数据处理器名（用于导出，如 `xml`）。
    - `type`：类型名。
    - `name`：表名（决定输出文件名）。
    - `offset`：起始偏移。
    - `length`：元素数量。
  - 返回：`0` 成功；`-10` 流无效；`-11` 处理器创建失败；`-12` 类型创建失败。
  - 内部处理：创建 `Sheet` 并读取流，输出到 `${mule.sheet.basedir}${name}.${handler}`。

- `importsht(hndl, handler, type, name, offset, length) -> int`
  - 参数与 `exportsht` 相同。
  - 返回：`0` 成功；`-10` 流无效；`-11` 处理器创建失败；`-12` 类型创建失败。
  - 内部处理：读取 `${mule.sheet.basedir}${name}.${handler}`，写回流。

- `shtload(hndl, name, type, offset, length) -> table/int`
  - 参数：同上。
  - 返回：成功时返回表格映射（Lua table）；失败返回负值。
  - 内部处理：读取 `Sheet` 并用 `Mappifier` 转为 Lua 表。

- `shtsave(hndl, value, type, offset, length) -> int`
  - 参数：
    - `value`：Lua 表（映射结构）。
  - 返回：`0` 成功；`-10` 流无效；`-11` 类型创建失败；`-12` 参数不合法。
  - 内部处理：将 `value` 写回流。

- `shtloadfile(name, type, handler) -> table`
  - 参数：
    - `name`：表名（文件名）。
    - `type`：类型名（当前实现未使用）。
    - `handler`：处理器名（决定文件后缀）。
  - 返回：解析结果的 Lua 表。
  - 内部处理：读取 `${mule.sheet.basedir}${name}.${handler}` 并解析为多值结构。

- `shtldf(name, type, handler) -> table`
  - `shtloadfile` 的别名。

#### 表注册与批量导入导出

- `shtreg(hndl, name, type, offset, length) -> int`
  - 参数：
    - `hndl`：流句柄。
    - `name`：表名。
    - `type`：类型名。
    - `offset`：起始偏移。
    - `length`：元素数量。
  - 返回：`0` 成功；`-10` 流无效。
  - 内部处理：注册 `Sheet` 到 `SheetManager`。

- `shtrm(hndl, name) -> int`
  - 参数：
    - `hndl`：流句柄。
    - `name`：表名。
  - 返回：`0` 成功；`-10` 流无效。
  - 内部处理：移除指定表。

- `shtclr(hndl) -> int`
  - 参数：
    - `hndl`：流句柄。
  - 返回：`0` 成功；`-10` 流无效。
  - 内部处理：清空该流已注册的所有表。

- `shtex(hndl, handler) -> int`
  - 参数：
    - `hndl`：流句柄。
    - `handler`：数据处理器名。
  - 返回：`0` 成功；`-10` 流无效。
  - 内部处理：对该流注册的所有表调用导出（`SheetManager::ReadSheets`）。

- `shtim(hndl, handler) -> int`
  - 参数：
    - `hndl`：流句柄。
    - `handler`：文件处理器名。
  - 返回：`0` 成功；`-10` 流无效。
  - 内部处理：对该流注册的所有表调用导入（`SheetManager::WriteSheets`）。

#### 使用示例

```lua
-- 注册表并批量导出
shtreg(0, "ToC", "FileEntry", 0x100, 100)
shtex(0, "xml")
```

```lua
-- 读取单表到 Lua
define("FileEntry", { "offset:int32", "length:int32", "name:string*" })
local tbl = shtload(0, "ToC", "FileEntry", 0x100, 100)

```

### 智能引用记忆（SmartReference）

该组函数管理流内的空闲片段记录，用于引用/分配空间。文件 `fileId` 位于 `mule.data.basedir` 的数据文件目录。

- `savemem() -> int`
  - 参数：无。
  - 返回：`0` 成功。
  - 内部处理：保存全局空闲空间信息（`MemoryManager::SaveFreeSpace`）。

- `ldmem(hndl, fileId) -> int`
  - 参数：
    - `hndl`：流句柄。
    - `fileId`：数据文件编号。
  - 返回：`0` 成功；负值表示失败。
  - 内部处理：从数据文件读取片段列表并注册到内存管理器。

- `srmload(hndl, fileId) -> int`
  - `ldmem` 的别名。

- `srmsave(hndl, fileId) -> int`
  - 参数：
    - `hndl`：流句柄。
    - `fileId`：数据文件编号。
  - 返回：`0` 成功；负值表示失败。
  - 内部处理：将指定流的空闲片段列表保存到数据文件。

- `srmreg(hndl, position, size) -> int`
  - 参数：
    - `hndl`：流句柄。
    - `position`：空闲片段起始偏移。
    - `size`：空闲片段大小。
  - 返回：`0` 成功；负值表示失败。
  - 内部处理：注册空闲片段。

- `srmalloc(hndl, size, align) -> int`
  - 参数：
    - `hndl`：流句柄。
    - `size`：申请大小。
    - `align`：对齐要求。
  - 返回：分配到的偏移（失败返回 `0`）。
  - 内部处理：在已注册片段中分配空间。

#### 使用示例

```lua
-- 加载空闲片段信息并申请空间
srmload(0, 1)
local pos = srmalloc(0, 0x100, 0x10) -- 申请 0x100 字节空间，16 字节对齐
if pos ~= 0 then
-- 成功申请到空间，注册该片段并保存回数据文件
  srmreg(0, pos, 0x100)
  srmsave(0, 1)
end
-- （以上操作是空操作，仅作示例）
```

### 转换与编码

该组函数依赖插件提供的转换器，用于在二进制流与文本之间转换。

- `cvttxt(hndl, converter, output, param) -> int`
  - 参数：
    - `hndl`：流句柄。
    - `converter`：转换器名称。
    - `output`：输出文件相对路径（拼接到 `mule.resource.basedir`）。
    - `param`：转换器参数字符串（由插件解释）。
  - 返回：`0` 成功。
  - 内部处理：读取流并使用转换器输出文本。

- `cvtbin(text, converter, hndl, param) -> int`
  - 参数：
    - `text`：输入文本文件相对路径（拼接到 `mule.resource.basedir`）。
    - `converter`：转换器名称。
    - `hndl`：流句柄。
    - `param`：转换器参数字符串。
  - 返回：`0` 成功。
  - 内部处理：读取文本并写入流。

- `codemap(code) -> int`
  - 参数：
    - `code`：码表资源 ID，`0` 表示重置。
  - 返回：`0` 成功。
  - 内部处理：加载并初始化编码转换表。

- `crc32(path) -> int`
  - 参数：
    - `path`：虚拟文件路径（`root:/path`）。
  - 返回：计算得到的 CRC32。
  - 内部处理：读取文件并计算 CRC32。

#### 使用示例

```lua
-- 二进制转文本
cvttxt(0, "xml", "exports/foo.xml", "")

-- 文本转二进制
cvtbin("exports/foo.xml", "xml", 0, "")
```
