# Mule 主程序使用文档

## 目标与读者
本手册面向首次使用 Mule 进行数据导入/导出与格式转换的用户。内容按照“准备工作 → 编写脚本 → 定义结构 → 执行导出/导入”的顺序组织，以便跟随步骤完成一个完整流程。

## 快速开始（完成一次导出/导入）
1. 准备最小工作区（含 `resources/scripts/` 与可选的 `resources/definition/`）。
2. 编写 Lua 脚本，描述目标对象与读写方式。
3. 编写结构定义 XML（若需要结构化导出/导入）。
4. 使用命令行执行导出/导入。
5. 验证结果并迭代脚本或结构。

## 命令行参数
```text
mule <path-to-work-area> <path-to-target-file> <action> [options]
```
- `path-to-work-area`：工作区路径。
- `path-to-target-file`：目标文件或目录路径。
- `action`：动作名称。若为 `interact` 则进入交互模式，否则会执行 `resources/scripts/<action>.lua`。
- `options`：
  - `-l`：更少日志输出（less log）。
  - `--log-file=<file>`：将日志输出到指定文件。
  - `-d<name>=<value>`：定义配置变量（写入 `Configuration`）。

## 工作区结构
### 最小工作区
运行时会按以下目录约定构造路径：
- `mule.basedir = <path-to-work-area>/`
- `mule.resource.basedir = <path-to-work-area>/resources/`
- `mule.script.basedir = <path-to-work-area>/resources/scripts/`
- `mule.data.basedir = <path-to-work-area>/data/`
- `mule.sheet.basedir = <path-to-work-area>/sheets/`

推荐的最小目录结构：
```text
<work-area>/
  resources/
    scripts/
      config.lua
      <action>.lua
    definition/
  data/
  sheets/
```

## 运行时行为（重要）
1. 程序启动后会设置 `mule.target` 与 `mule.target.type`（`file` 或 `directory`）。
2. 若目标是目录，将其挂载到虚拟文件系统根 `target:`；若目标是文件，则以二进制流形式提供给 Lua（句柄 `0`）。
3. 总是先执行 `resources/scripts/config.lua`，再执行 `<action>.lua`。

## 编写脚本（Lua）
脚本负责：
- 定义输入/输出流。
- 选择导出或导入流程。
- 绑定结构定义（如 XML）。

### 脚本运行环境
Lua 运行时加载的标准库模块包括 `base`、`package`、`table`、`string`、`math`、`utf8`、`debug`。

### Lua API 速查
更完整的 API 说明请参阅 [LuaAPI](LuaAPI.md)。

**流与文件：**
- `open` / `close` / `read` / `write` / `tell` / `seek`
- `readbyte`/`writebyte`、`readu16`/`writeu16`、`readu32`/`writeu32`、`readu64`/`writeu64`
- `readi16`/`writei16`、`readi32`/`writei32`、`readi64`/`writei64`
- `readfloat`/`writefloat`、`readdouble`/`writedouble`

**数据导入/导出：**
- `export` / `extract` / `import` / `patch`
- `resexport` / `resimport`

**容器与虚拟文件系统：**
- `mount` / `openpack` / `unmount` / `list` / `listroots` / `remove` / `mdata`

**结构与表格：**
- `define` / `loaddef`
- `shtreg` / `shtrm` / `shtclr` / `shtex` / `shtim`
- `exportsht` / `importsht` / `shtload` / `shtsave` / `shtloadfile` / `shtldf`

**转换与编码：**
- `cvttxt` / `cvtbin` / `codemap` / `crc32`

**智能引用记忆：**
- `savemem` / `srmload` / `srmsave` / `srmreg` / `srmalloc`

**系统与配置：**
- `log` / `loginit` / `logsetout` / `logseterr`
- `config` / `config_erase`
- `confirm` / `lplugin` / `pplugin`

### 针对单文件的脚本编写
**场景**：有一个数据文件，需要导出为 XML 并支持导回。

**导出示例脚本**（`resources/scripts/export.lua`）：

```lua
-- 使用文件句柄 0 访问目标文件（Mule 自动将目标文件打开为流 0）
-- 如果需要，可以检查目标类型
if mule.target.type ~= "file" then
    log("错误：此脚本仅支持单文件目标")
    return
end

-- 定义数据结构
loaddef("gamedata")  -- 从 resources/definition/gamedata.xml 加载结构定义

-- 或者使用 Lua 代码直接定义结构：
define("ItemData", {
    "id:uint32",
    "name:varchar(32)",
    "price:uint32",
    "type:uint16",
    "rarity:uint8",
    "padding:dum8"  -- 填充字节，跳过 1 字节
})

-- 注册需要导出的表
-- 参数：流句柄, 表名, 结构类型, 数据起始偏移, 元素数量
shtreg(0, "items", "ItemData", 0x0, 100)
shtreg(0, "skills", "SkillData", 0x5000, 50)
shtreg(0, "characters", "CharacterData", 0x8000, 20)

-- 可以继续注册其他表...

-- 执行导出，使用 XML 格式
shtex(0, "xml")
-- 将导出到 sheets/items.xml, sheets/skills.xml, sheets/characters.xml

log("导出完成")
close(0)
```

**导入示例脚本**（`resources/scripts/import.lua`）：

```lua
-- 检查目标类型
if mule.target.type ~= "file" then
    log("错误：此脚本仅支持单文件目标")
    return
end

-- 加载结构定义（必须与导出时相同）
loaddef("gamedata")

-- 注册需要导入的表（与导出时相同）
shtreg(0, "items", "ItemData", 0x0, 100)
shtreg(0, "skills", "SkillData", 0x5000, 50)
shtreg(0, "characters", "CharacterData", 0x8000, 20)

-- 执行导入，使用 XML 格式
shtim(0, "xml")
-- 将从 sheets/items.xml, sheets/skills.xml, sheets/characters.xml 读取数据并写入

log("导入完成")
close(0)
```

**结构定义示例**（`resources/definition/gamedata.xml`）：

```xml
<def version="1.1">
  <!-- 道具数据结构 -->
  <struct name="ItemData">
    <field name="id" type="uint32" />
    <field name="name" type="varchar(32)" />
    <field name="price" type="uint32" />
    <field name="type" type="uint16" />
    <field name="rarity" type="uint8" />
    <field name="padding" type="dum8" />
  </struct>
  
  <!-- 技能数据结构 -->
  <struct name="SkillData">
    <field name="id" type="uint32" />
    <field name="name" type="varchar(64)" />
    <field name="power" type="uint16" />
    <field name="mpCost" type="uint16" />
    <field name="description" type="varchar(128)" />
  </struct>
  
  <!-- 角色数据结构 -->
  <struct name="CharacterData">
    <field name="id" type="uint32" />
    <field name="name" type="varchar(32)" />
    <field name="level" type="uint8" />
    <field name="hp" type="uint16" />
    <field name="mp" type="uint16" />
    <field name="strength" type="uint16" />
    <field name="defense" type="uint16" />
  </struct>
</def>
```

**导出后的 XML 文件示例**（`sheets/items.xml`）：

```xml
<items>
  <item>
    <id>1</id>
    <name>回复药</name>
    <price>50</price>
    <type>1</type>
    <rarity>0</rarity>
    <padding>0</padding>
  </item>
  <item>
    <id>2</id>
    <name>魔法药</name>
    <price>80</price>
    <type>1</type>
    <rarity>0</rarity>
    <padding>0</padding>
  </item>
  <!-- 更多道具数据... -->
</items>
```

**命令行执行**：

导出数据：
```bash
mule ./workspace /path/to/game.dat export
```

修改 `./workspace/sheets/` 目录下的 XML 文件后，导入数据：
```bash
mule ./workspace /path/to/game.dat import
```

**配置文件示例**（`resources/scripts/config.lua`）：

```lua
-- 配置文件会在执行 action 脚本之前运行
-- 可以在这里设置全局配置

-- 设置日志级别（0=Info, 1=Note, 2=Warn, 3=Error, 4=Fatal, -1=Debug）
-- loginit(0, true)  -- 第一个参数是日志级别，第二个是是否启用彩色输出

-- 设置智能引用的对齐值
-- config("mule.data.smart-reference.align", 4)

-- 打印目标信息
log("目标文件: " .. mule.target.name)
log("目标类型: " .. mule.target.type)
```

**注意事项**：
- 文件句柄 `0` 由 Mule 自动打开，表示命令行指定的目标文件
- 使用 `close(0)` 关闭文件后，无法再访问该文件
- 数据偏移和元素数量需要根据实际文件格式确定
- 若有零结尾字符串存在对齐要求，当前暂时无法处理，你需要编写自己的插件来处理这种字符串（见下文 C++ 插件部分）
- `dum8`/`dum16`/`dum32`/`dum64` 用于跳过填充字节
- CSV 格式也可用于导入导出，只需将 `"xml"` 改为 `"csv"`

### 针对文件容器（单文件目标）的脚本编写
**场景**：有一个 ISO 镜像，需要从其中的 `MAIN.ELF;1` 导出特定区段和一段数据，结构化为 XML。

**步骤**：
1. 使用文件句柄 0 将目标文件挂载为 "iso" 类型的容器。
2. 使用容器插件将镜像挂载为虚拟文件系统。
3. 打开容器内目标文件并执行导出逻辑。

```lua
-- 将流 0（即目标文件）挂载为 "iso" 类型的容器，根路径为 "iso:"，之后可以通过 "iso:/MAIN.ELF;1" 访问。
mount(0, "iso", "iso")
local stream = open("iso:/MAIN.ELF;1", "r")
-- 将流 stream 通过 "elf" 插件处理，得到一个新的流 elf。此 elf 流会改变 seek 的行为
-- 在 elf 上 seek 时，会根据程序头定义的 PT_LOAD，以 VMA 的方式 seek_
local elf = sos(stream, "elf")

-- 通过 stream 访问原始数据：
seek(stream, 0x1234, 0) -- 从文件开头偏移 0x1234
-- 通过 elf 访问虚拟地址空间：
seek(elf, 0x5678, 0) -- 从虚拟地址空间偏移 0x5678

-- 从 resources/definition/ 中加载结构定义 XML，绑定到当前环境。
loaddef ("file") -- 将读取 resources/definition/file.xml 的内容并解析为结构定义
-- 也可以通过 Lua 代码直接创建结构定义：
define("fileToc", { "filename:string", "offset:uint32", "size:uint32" })

-- 将表注册到流 elf 上，表名为 "toc"，结构为 "fileToc"，VMA 为 0x1234，元素数量为 10。
shtreg(elf, "toc", "fileToc", 0x1234, 10)
shtreg(elf, "data", "fileData", 0x5678, 20)
-- 继续注册其他表...

-- 实行导出，使用 XML 格式
shtex(elf, "xml")
-- 将导出到 sheets/fileToc.xml, sheets/fileData.xml 等文件中，文件名由表名组成。

-- 可以删除某个表：
shtrm(elf, "toc")

-- 可以清除所有注册的表：
shtclr(elf)

-- 也可以一次导出一个表，不使用 shtreg 注册：
exportsht(elf, "xml", "fileToc", "TOC", 0x1234, 10) -- 此操作会即刻导出表 TOC 到 sheets/TOC.xml 中，并且不在 elf 上注册表 "fileToc"。

-- 使用了智能指针的情况，智能指针会在导出时记录哪些空间是被这类指针所管理的
-- 为了在导入时正确地分配空间并回写指针地址，必须先保存智能引用记忆：
srmsave(elf, 0x85001235) -- 将智能引用记忆保存到 data/85/00/12/35.DAT 中
-- 注：数据文件 ID 可以自定义，但必须大于0x10000000，低于0xFFFFFFFF，且在同一导出/导入流程中保持一致。

-- 最后别忘了关闭流：
close(elf)
close(stream)
unmount("iso") -- 卸载容器，确保对应的文件系统元数据被正确写回
close(0) -- 关闭流 0，即目标文件的二进制流，注意，此后不再能访问目标文件了。
```

XML 定义文件的示例：
```xml
<def version="1.1">
  <struct name="fileToc">
    <field name="filename" type="string*" />
    <field name="offset" type="uint32" />
    <field name="size" type="uint32" />
  </struct>
</def>
```

**导入示例**：

导入过程与导出类似，主要区别在于：
1. 打开模式改为读写（`"rw"` 或 `"w"`）（由于 ELF 插件需要读取原始数据以计算地址，因此必须使用读写模式 rw 打开）。
2. 使用 `shtim` 替代 `shtex`
3. 若使用智能引用，需先加载记忆文件

```lua
-- 导入脚本示例（import.lua）
-- 将修改后的 XML 数据导入回 ISO 镜像中的 ELF 文件

-- 以读写模式打开目标文件
mount(0, "iso", "iso")
local stream = open("iso:/MAIN.ELF;1", "rw")  -- 注意这里使用读写模式
local elf = sos(stream, "elf") -- 必须可读，ELF 插件需要读取原始数据以计算地址

-- 加载结构定义（与导出时相同）
loaddef("file")

-- 若导出时使用了智能引用，需要先加载智能引用记忆
-- 这样系统才知道哪些空间是可用的，哪些已被占用
srmload(elf, 0x85001235)  -- 加载之前保存的智能引用记忆（注意数据文件 ID 必须与导出时一致）

-- 注册需要导入的表（与导出时相同）
shtreg(elf, "toc", "fileToc", 0x1234, 10)
shtreg(elf, "data", "fileData", 0x5678, 20)

-- 执行导入，使用 XML 格式
shtim(elf, "xml")
-- 将从 sheets/toc.xml, sheets/data.xml 等文件读取数据并写回二进制流

-- 关闭流以确保数据写入
close(elf)
close(stream)
unmount("iso")
close(0)
```

**命令行执行示例**：

导出数据：
```bash
mule ./workspace /path/to/game.iso export
```

修改 `./workspace/sheets/` 目录下的 XML 文件后，导入数据：
```bash
mule ./workspace /path/to/game.iso import
```

**注意事项**：
- 导入时，一般需要 rw 模式打开流，不止 ELF，部分数据类型也可能需要读取原始数据以计算地址或验证结构。
- 字符串长度不能超过导出时的大小限制
- 若使用智能引用（`类型^`），必须先加载记忆文件（`srmload`）
- 导入完成后需保存智能引用记忆（`srmsave`）以便下次使用
- XML 中的数据必须符合结构定义，否则会导致导入失败

### 针对文件夹目标的脚本编写
**场景**：一个文件夹中包含多个数据文件，需要批量导出为 XML。

当目标为目录时，Mule 会将其挂载到虚拟文件系统根 `target:`，可以通过 `list("target")` 遍历目录下的所有文件。

**批量导出示例脚本**（`resources/scripts/batch_export.lua`）：

```lua
-- 检查目标类型
if mule.target.type ~= "directory" then
    log("错误：此脚本仅支持目录目标")
    return
end

-- 加载结构定义
loaddef("gamedata")

-- 获取目标目录下的所有文件
local fileList = list("target")
log("找到 " .. #fileList .. " 个文件")

-- 遍历文件列表
for i = 1, #fileList do
    local filePath = fileList[i]
    log("处理文件: " .. filePath)
    
    -- 过滤：只处理 .dat 文件
    if string.match(filePath, "%.dat$") then
        -- 提取文件名（不含路径和扩展名）
        local fileName = string.match(filePath, "([^/\\]+)%.dat$")
        
        -- 打开文件
        local stream = open("target:/" .. filePath, "r")
        if stream then
            -- 根据文件名或内容决定如何处理
            -- 这里假设所有 .dat 文件都有相同的结构
            
            -- 注册表，使用文件名作为表名前缀
            shtreg(stream, fileName .. "_items", "ItemData", 0x0, 100)
            
            -- 导出为 XML
            shtex(stream, "xml")
            
            -- 关闭文件
            close(stream)
            
            log("完成: " .. fileName)
        else
            log("警告：无法打开文件 " .. filePath)
        end
    end
end

log("批量导出完成")
```

**更复杂的批量处理示例**：

```lua
-- 批量处理不同类型的文件
if mule.target.type ~= "directory" then
    log("错误：此脚本仅支持目录目标")
    return
end

loaddef("gamedata")

-- 定义文件处理规则
local fileHandlers = {
    -- 道具数据文件
    ["items%.dat$"] = function(stream, fileName)
        shtreg(stream, fileName .. "_items", "ItemData", 0x0, 100)
        shtex(stream, "xml")
    end,
    
    -- 技能数据文件
    ["skills%.dat$"] = function(stream, fileName)
        shtreg(stream, fileName .. "_skills", "SkillData", 0x0, 50)
        shtex(stream, "xml")
    end,
    
    -- 角色数据文件
    ["characters%.dat$"] = function(stream, fileName)
        shtreg(stream, fileName .. "_characters", "CharacterData", 0x0, 20)
        shtex(stream, "xml")
    end,
}

-- 获取所有文件
local fileList = list("target")

-- 遍历并处理文件
for i = 1, #fileList do
    local filePath = fileList[i]
    local fileName = string.match(filePath, "([^/\\]+)$")  -- 提取文件名
    
    -- 尝试匹配处理规则
    local handled = false
    for pattern, handler in pairs(fileHandlers) do
        if string.match(fileName, pattern) then
            log("处理: " .. filePath)
            local stream = open("target:/" .. filePath, "r")
            if stream then
                -- 调用对应的处理函数
                handler(stream, string.gsub(fileName, "%.dat$", ""))
                close(stream)
                handled = true
                break
            end
        end
    end
    
    if not handled and string.match(fileName, "%.dat$") then
        log("跳过未知文件: " .. fileName)
    end
end

log("批量处理完成")
```

**批量导入示例脚本**（`resources/scripts/batch_import.lua`）：

```lua
-- 批量导入：将修改后的 XML 文件导入回对应的 .dat 文件
if mule.target.type ~= "directory" then
    log("错误：此脚本仅支持目录目标")
    return
end

loaddef("gamedata")

-- 获取所有需要导入的文件（这里假设已知文件列表）
local filesToImport = {
    "data/items.dat",
    "data/skills.dat",
    "data/characters.dat",
}

for i = 1, #filesToImport do
    local filePath = filesToImport[i]
    local fileName = string.match(filePath, "([^/\\]+)%.dat$")
    
    log("导入: " .. filePath)
    
    -- 以读写模式打开文件
    local stream = open("target:/" .. filePath, "rw")
    if stream then
        -- 注册表（与导出时相同）
        if fileName == "items" then
            shtreg(stream, fileName .. "_items", "ItemData", 0x0, 100)
        elseif fileName == "skills" then
            shtreg(stream, fileName .. "_skills", "SkillData", 0x0, 50)
        elseif fileName == "characters" then
            shtreg(stream, fileName .. "_characters", "CharacterData", 0x0, 20)
        end
        
        -- 执行导入
        shtim(stream, "xml")
        
        close(stream)
        log("完成: " .. fileName)
    else
        log("警告：无法打开文件 " .. filePath)
    end
end

log("批量导入完成")
```

**使用模块化组织的示例**（`resources/scripts/lib/file_processor.lua`）：

```lua
-- 文件处理器模块
local M = {}

-- 处理单个文件的通用函数
function M.processFile(filePath, tableName, typeName, offset, count, mode)
    local stream = open("target:/" .. filePath, mode or "r")
    if not stream then
        log("错误：无法打开文件 " .. filePath)
        return false
    end
    
    shtreg(stream, tableName, typeName, offset, count)
    
    if mode == "r" or not mode then
        shtex(stream, "xml")
    elseif mode == "rw" then
        shtim(stream, "xml")
    end
    
    close(stream)
    return true
end

-- 批量处理文件
function M.processBatch(fileConfigs, mode)
    local successCount = 0
    local failCount = 0
    
    for i = 1, #fileConfigs do
        local config = fileConfigs[i]
        log("处理 [" .. i .. "/" .. #fileConfigs .. "]: " .. config.path)
        
        if M.processFile(config.path, config.tableName, config.typeName, 
                         config.offset, config.count, mode) then
            successCount = successCount + 1
        else
            failCount = failCount + 1
        end
    end
    
    log(string.format("完成：成功 %d，失败 %d", successCount, failCount))
end

return M
```

**使用模块的主脚本**（`resources/scripts/export_all.lua`）：

```lua
-- 使用模块化处理
local processor = require("lib.file_processor")

if mule.target.type ~= "directory" then
    log("错误：此脚本仅支持目录目标")
    return
end

loaddef("gamedata")

-- 定义文件配置
local fileConfigs = {
    {
        path = "data/items.dat",
        tableName = "items_data",
        typeName = "ItemData",
        offset = 0x0,
        count = 100
    },
    {
        path = "data/skills.dat",
        tableName = "skills_data",
        typeName = "SkillData",
        offset = 0x0,
        count = 50
    },
    {
        path = "data/characters.dat",
        tableName = "characters_data",
        typeName = "CharacterData",
        offset = 0x0,
        count = 20
    },
}

-- 批量导出
processor.processBatch(fileConfigs, "r")
```

**命令行执行**：

批量导出：
```bash
mule ./workspace /path/to/game_data/ batch_export
```

批量导入：
```bash
mule ./workspace /path/to/game_data/ batch_import
```

使用模块化脚本：
```bash
mule ./workspace /path/to/game_data/ export_all
```

**注意事项**：
- `list("target")` 返回的是完整路径列表（包含所有子目录的文件）
- 文件路径分隔符依赖于操作系统，建议使用 `/` 作为统一分隔符，Windows 也支持 `/`
- 使用 `open("target:/path/to/file", mode)` 打开目录中的文件
- 建议使用模式匹配（`string.match`）来过滤和识别文件类型
- 可以使用 `require` 加载自定义 Lua 模块来组织代码
- 批量处理时建议添加错误处理，避免单个文件失败导致整个流程中断
- 对于大量文件，可以添加进度显示和日志记录

### 将整个文件导入/导出
**场景**：需要将整个文件直接从文件包中抽出编辑，或将一个文件直接打包回去。

**限制**：Mule 在导入时，视乎文件包处理逻辑，可能无法将大于原始文件大小的内容写回，因此只适用于小文件或不修改大小的场景。若需要修改文件大小，建议使用专门的文件包编辑工具或编写支持修改大小的插件。

**API 说明**：

Mule 提供了两组文件导入/导出 API：

1. **数据文件系统**（`data/` 目录）：
   - `export(path, id)` - 导出整个文件到数据文件
   - `extract(path, offset, length, id)` - 导出文件的一部分到数据文件
   - `import(path, id)` - 从数据文件导入整个文件
   - `patch(path, offset, length, id)` - 从数据文件导入到文件的特定位置

2. **资源文件系统**（`resources/` 目录）：
   - `resexport(srcPath, destPath)` - 导出文件到资源目录
   - `resimport(destPath, srcPath)` - 从资源目录导入文件

**数据文件 ID 规则**：
- 范围：`0x01000000` - `0x09FFFFFF`（保留给用户使用）
- `0x00000000` - `0x00FFFFFF`：系统保留
- `0x10000000` - `0xFFFFFFFF`：可用于智能引用记忆等临时数据

**导出整个文件示例**（`resources/scripts/extract_files.lua`）：

```lua
-- 从 ISO 镜像中导出多个文件到数据目录
if mule.target.type ~= "file" then
    log("错误：此脚本仅支持文件目标")
    return
end

-- 挂载 ISO 镜像
mount(0, "iso", "iso")

-- 导出文件列表
local filesToExport = {
    {path = "iso:/SYSTEM/MAIN.ELF;1", id = 0x01000001},
    {path = "iso:/DATA/CONFIG.DAT;1", id = 0x01000002},
    {path = "iso:/DATA/ITEMS.BIN;1", id = 0x01000003},
}

for i = 1, #filesToExport do
    local file = filesToExport[i]
    log("导出: " .. file.path .. " -> " .. string.format("0x%08X", file.id))
    
    -- export 会自动打开文件、读取全部内容并保存到 data/ 目录
    export(file.path, file.id)
    
    log("完成: " .. file.path)
end

log("所有文件导出完成")
unmount("iso")
close(0)
```

**导入整个文件示例**（`resources/scripts/inject_files.lua`）：

```lua
-- 将数据目录中的文件导入回 ISO 镜像
if mule.target.type ~= "file" then
    log("错误：此脚本仅支持文件目标")
    return
end

-- 挂载 ISO 镜像（需要读写模式）
mount(0, "iso", "iso")

-- 导入文件列表（与导出时相同）
local filesToImport = {
    {path = "iso:/SYSTEM/MAIN.ELF;1", id = 0x01000001},
    {path = "iso:/DATA/CONFIG.DAT;1", id = 0x01000002},
    {path = "iso:/DATA/ITEMS.BIN;1", id = 0x01000003},
}

for i = 1, #filesToImport do
    local file = filesToImport[i]
    log("导入: " .. string.format("0x%08X", file.id) .. " -> " .. file.path)
    
    -- import 会从 data/ 目录读取文件并写入目标位置
    import(file.path, file.id)
    
    log("完成: " .. file.path)
end

log("所有文件导入完成")
unmount("iso")
close(0)
```

**部分导出/导入示例**：

```lua
-- 导出文件的特定部分
mount(0, "iso", "iso")

-- 导出 MAIN.ELF 的 0x1000 - 0x5000 区域
extract("iso:/SYSTEM/MAIN.ELF;1", 0x1000, 0x4000, 0x01000010)

-- 导入时覆盖到相同位置
-- patch("iso:/SYSTEM/MAIN.ELF;1", 0x1000, 0x4000, 0x01000010)

unmount("iso")
close(0)
```

**使用资源目录的示例**：

```lua
-- 导出到资源目录（更方便直接编辑）
mount(0, "iso", "iso")

-- 导出到 resources/exported/config.dat
resexport("iso:/DATA/CONFIG.DAT;1", "exported/config.dat")

-- 导入回去
-- resimport("iso:/DATA/CONFIG.DAT;1", "exported/config.dat")

unmount("iso")
close(0)
```

**批量导出/导入目录中的所有文件**：

```lua
-- 批量导出文件夹中的所有文件
if mule.target.type ~= "directory" then
    log("错误：此脚本仅支持目录目标")
    return
end

local fileList = list("target")
local baseId = 0x01000000  -- 起始 ID

for i = 1, #fileList do
    local filePath = fileList[i]
    local fileId = baseId + i
    
    log(string.format("导出 [%d/%d]: %s -> 0x%08X", i, #fileList, filePath, fileId))
    
    -- 导出文件
    export("target:/" .. filePath, fileId)
end

log(string.format("导出完成，共 %d 个文件", #fileList))
```

**使用映射文件管理导出**：

```lua
-- 导出时生成映射文件
mount(0, "iso", "iso")

local files = {}
local startId = 0x01000000

-- 遍历需要导出的文件
local paths = list("iso")
for i = 1, #paths do
    local path = paths[i]
    local id = startId + i
    
    -- 导出文件
    export("iso:/" .. path, id)
    
    -- 记录映射关系
    files[i] = {path = path, id = id}
    log(string.format("导出: %s -> 0x%08X", path, id))
end

-- 保存映射文件到资源目录（使用 JSON 或简单文本格式）
local mapFile = open("target:/file_map.txt", "w")
if mapFile then
    for i = 1, #files do
        -- 写入映射信息（简单的文本格式）
        -- TODO: 实际使用时需要实现文本写入功能或使用其他方式
        log(string.format("映射: %s = 0x%08X", files[i].path, files[i].id))
    end
    close(mapFile)
end

unmount("iso")
close(0)
```

**数据文件存储位置**：

导出的文件存储在 `data/` 目录下，路径结构为：
```
data/
  01/
    00/
      00/
        01.dat  -- ID 0x01000001
        02.dat  -- ID 0x01000002
```

**命令行执行**：

导出文件：
```bash
mule ./workspace /path/to/game.iso extract_files
```

导入文件：
```bash
mule ./workspace /path/to/game.iso inject_files
```

**注意事项**：

1. **文件大小限制**：
   - `import` 使用 `FOM_WRITE | FOM_TRUNCATE` 模式，会截断原文件
   - 如果新文件大于原文件，可能无法写入（取决于文件包实现）
   - 建议只修改内容而不改变文件大小

2. **数据文件 ID 管理**：
   - 使用 `0x01000000` - `0x09FFFFFF` 范围避免与系统冲突
   - 建议维护一个映射表记录 ID 与文件路径的对应关系
   - 可以使用文件 CRC32 或路径 hash 作为 ID

3. **资源目录 vs 数据目录**：
   - `resexport`/`resimport`：文件存储在 `resources/` 目录，便于手动编辑
   - `export`/`import`：文件存储在 `data/` 目录，使用数字 ID 管理

4. **错误处理**：
   - `import` 会在失败时抛出异常
   - 可以使用 Lua 的 `pcall` 捕获错误
   - 批量操作时建议添加错误处理避免中断

5. **ISO 文件特殊性**：
   - ISO 容器支持文件修改，但有大小限制
   - 修改后的文件会更新 ISO 目录项的时间戳
   - 增大的文件可能需要重新构建 ISO (Mule 无法处理)

**示例：使用 CRC32 作为文件 ID**：

```lua
-- 使用 CRC32 作为文件 ID
mount(0, "iso", "iso")

local paths = list("iso")
for i = 1, #paths do
    local path = paths[i]
    
    -- 计算文件的 CRC32 作为 ID
    local fileId = crc32("iso:/" .. path)
    
    -- 确保 ID 在有效范围内
    fileId = (fileId & 0x0FFFFFFF) | 0x10000000
    
    log(string.format("导出: %s -> 0x%08X (CRC32)", path, fileId))
    export("iso:/" .. path, fileId)
end

unmount("iso")
close(0)
```

## 结构定义 XML 编写
结构定义用于描述二进制数据的布局与字段类型，配合导出/导入实现结构化转换。

**基本要素**：
- 根节点与结构体命名。
- 字段类型（如 `int32`、`string*`）。
- 对齐、偏移、条件字段等。

示例：
### 最小可用结构定义示例
```xml
<def version="1.1">
  <struct name="FileEntry">
    <field name="name" type="string*" />
    <field name="offset" type="uint32" />
    <field name="size" type="uint32" />
  </struct>
</def>
```

### 常用类型列表
**基本整数类型**：
- `int8`, `int16`, `int32`, `int64`：有符号整数
- `uint8`, `uint16`, `uint32`, `uint64`：无符号整数
- `dum8`, `dum16`, `dum32`, `dum64`：占位符类型（跳过指定字节数）

**浮点类型**：
- `float`：单精度浮点数（32位）
- `double`：双精度浮点数（64位）

**字符串类型**：
- `string`：以 null 结尾的字符串（本地编码，通常为 UTF-8 或系统默认编码）
- `u8string`：以 null 结尾的 UTF-8 编码字符串
- `u16string`：以 null 结尾的 UTF-16 编码字符串
- `u32string`：以 null 结尾的 UTF-32 编码字符串
- `varchar(n)`：固定长度字符串缓冲区（n 字节），不足部分填充 null

**引用类型（智能指针）**：
- `类型^`：智能引用，例如 `string^`, `uint32^`。读取时从指针位置读取数据，写入时自动分配空间并记录指针。

**数组类型**：
数组通过在字段元信息中指定长度来定义，而不是在类型名中指定。

**枚举类型**：
通过 `<enum>` 标签定义，需指定基础类型和枚举值。

### 结构定义示例（包含元信息）
```xml
<def version="1.1">
  <struct name="FileHeader">
    <field name="magic" type="uint32" />
    <field name="version" type="uint16" />
    <field name="entryCount" type="uint16" />
  </struct>
  
  <enum name="FileType" base="uint32">
    <i value="0">TYPE_UNKNOWN</i>
    <i value="1">TYPE_TEXT</i>
    <i value="2">TYPE_BINARY</i>
  </enum>
  
  <struct name="FileEntry">
    <field name="type" type="FileType" />
    <field name="offset" type="uint32" />
    <field name="size" type="uint32" />
    <field name="name" type="string" />
  </struct>
  
  <struct name="SmartRefExample">
    <field name="dataPtr" type="string^" />
    <field name="count" type="uint32" />
  </struct>
</def>
```

**注意**：`loaddef` 接受定义文件的名称（不含 `.xml` 扩展名），会自动从 `resources/definition/` 目录读取。例如 `loaddef("file")` 将读取 `resources/definition/file.xml`。

## 导入和导出数据
### 导出流程
1. 准备脚本与结构定义。
2. 运行命令行执行导出。

### 导入流程
1. 修改导出的 XML 或其他结构化文件。
2. 运行命令行执行导入。

## 细节
### 脚本细节
详见 [LuaAPI](LuaAPI.md) 文档。

### 结构定义 XML 细节

结构定义 XML 用于描述二进制数据的布局，Mule 通过解析这些定义来正确地读写复杂的数据结构。

#### XML 基本语法

**根元素**：
```xml
<def version="1.1">
  <!-- 定义内容 -->
</def>
```

- `version`：定义版本号，支持 `"1"` 和 `"1.1"`（推荐使用 `"1.1"`）
- 版本 1.1 新增了枚举类型支持

#### 结构体定义

**基本语法**：
```xml
<struct name="StructName">
  <field name="fieldName" type="fieldType" />
  <!-- 更多字段 -->
</struct>
```

**字段元信息**：

字段可以包含元信息来控制特殊行为：

```xml
<field name="count" type="uint32">
  <cache>itemCount</cache>  <!-- 将值缓存到上下文变量 itemCount -->
</field>

<field name="items" type="ItemData[itemCount]" />  <!-- 使用缓存的长度 -->
```

**常用元信息**：
- `<cache>变量名</cache>`：将字段值缓存到上下文变量中，供后续字段使用
- `<size>大小</size>`：指定字段的大小（某些类型需要）
- `<naming>模板</naming>`：用于表引用类型，指定生成的表名称模板

#### 数组类型

数组通过在类型名后添加 `[长度]` 来定义：

**固定长度数组**：
```xml
<field name="values" type="uint32[10]" />  <!-- 10 个 uint32 -->
```

**变长数组（使用上下文变量）**：
```xml
<struct name="DataBlock">
  <field name="count" type="uint32">
    <cache>elementCount</cache>  <!-- 缓存数量 -->
  </field>
  <field name="data" type="uint8[elementCount]" />  <!-- 使用缓存的数量 -->
</struct>
```

**无限长度数组（读到约束违反为止）**：
```xml
<field name="items" type="ItemData[]" />  <!-- 读到文件末尾或约束条件不满足 -->
```

#### 枚举类型

枚举类型在版本 1.1 中引入：

**基本语法**：
```xml
<enum name="EnumName" base="uint32">
  <i value="0">ENUM_VALUE_0</i>
  <i value="1">ENUM_VALUE_1</i>
  <i>ENUM_VALUE_2</i>  <!-- 自动递增，值为 2 -->
  <i>ENUM_VALUE_3</i>  <!-- 值为 3 -->
</enum>
```

**参数说明**：
- `name`：枚举类型名称
- `base`：基础类型（如 `uint8`、`uint16`、`uint32`、`uint64`、`int8`、`int16`、`int32`、`int64`）
- `value`：枚举值（可选，省略时自动递增）

**带约束的枚举**：
```xml
<enum name="FileType" base="uint32">
  <constraint>lt:10</constraint>  <!-- 值必须小于 10 -->
  <i value="0">TYPE_UNKNOWN</i>
  <i value="1">TYPE_TEXT</i>
  <i value="2">TYPE_BINARY</i>
</enum>
```

**约束条件**：
- `lt:值`：小于（less than）
- `nlt:值`：不小于（not less than，即大于等于）
- `gt:值`：大于（greater than）
- `ngt:值`：不大于（not greater than，即小于等于）
- `eq:值`：等于（equal）
- `neq:值`：不等于（not equal）

#### 嵌套结构

结构体可以嵌套使用：

```xml
<def version="1.1">
  <struct name="Point">
    <field name="x" type="float" />
    <field name="y" type="float" />
  </struct>
  
  <struct name="Rectangle">
    <field name="topLeft" type="Point" />
    <field name="bottomRight" type="Point" />
  </struct>
</def>
```

#### 智能引用（指针）

智能引用类型使用 `^` 后缀：

```xml
<struct name="StringTable">
  <field name="stringPtr" type="string^">  <!-- 智能引用到字符串 -->
    <cache>strPtr</cache>  <!-- 缓存指针值 -->
  </field>
  <field name="length" type="uint32">
    <cache>strLen</cache>  <!-- 缓存长度 -->
  </field>
</struct>
```

**智能引用的工作原理**：
1. **导出时**：读取指针指向的数据，记录指针管理的内存区域
2. **导入时**：在可用空间中分配内存，写入数据并更新指针

**智能引用的限制**：
- 需要配合 `srmload`/`srmsave` 使用
- 只能在支持的内存管理器中使用
- 导入时不能超过原始分配的空间大小

#### 表引用类型

表引用类型用于引用另一个表：

```xml
<struct name="FileEntry">
  <field name="dataOffset" type="uint32">
    <cache>dataPtr</cache>
  </field>
  <field name="dataSize" type="uint32">
    <cache>dataSize</cache>
  </field>
  <field name="data" type="uint8@dataPtr(dataSize)">
    <naming>${mule.data.sheet.name}/data_${mule.data.sheet.index}</naming>
  </field>
</struct>
```

**语法**：
- `类型@偏移指针(大小)`：定义表引用
- `类型@偏移指针`：不指定大小（使用默认值 1）
- `偏移指针` 和 `大小` 可以是上下文变量或常量

**命名模板变量**：
- `${mule.data.sheet.name}`：当前表名
- `${mule.data.sheet.index}`：当前元素索引
- `${offset}`：偏移地址（十六进制）
- `${stream}`：流名称
- 其他自定义配置变量

#### 包含其他定义文件

可以包含其他定义文件以重用定义：

```xml
<def version="1.1">
  <include ref="common_types" />  <!-- 包含 common_types.xml -->
  
  <struct name="MyData">
    <field name="header" type="CommonHeader" />  <!-- 使用包含的类型 -->
  </struct>
</def>
```

#### 完整示例

```xml
<def version="1.1">
  <!-- 包含通用类型定义 -->
  <include ref="base_types" />
  
  <!-- 枚举定义 -->
  <enum name="FileType" base="uint16">
    <constraint>lt:100</constraint>
    <i value="0">TYPE_UNKNOWN</i>
    <i value="1">TYPE_SCRIPT</i>
    <i value="2">TYPE_TEXTURE</i>
    <i value="3">TYPE_MODEL</i>
    <i value="4">TYPE_AUDIO</i>
  </enum>
  
  <!-- 简单结构体 -->
  <struct name="FileHeader">
    <field name="magic" type="uint32" />
    <field name="version" type="uint16" />
    <field name="fileCount" type="uint16">
      <cache>numFiles</cache>
    </field>
  </struct>
  
  <!-- 复杂结构体（包含数组和智能引用） -->
  <struct name="FileEntry">
    <field name="type" type="FileType" />
    <field name="nameOffset" type="uint32">
      <cache>namePtr</cache>
    </field>
    <field name="name" type="string^">
      <!-- 智能引用字符串，自动从 namePtr 读取 -->
    </field>
    <field name="dataOffset" type="uint32">
      <cache>dataPtr</cache>
    </field>
    <field name="dataSize" type="uint32">
      <cache>dataLen</cache>
    </field>
    <field name="flags" type="uint32" />
    <field name="padding" type="dum32" />  <!-- 跳过 4 字节 -->
  </struct>
  
  <!-- 包含数组的结构体 -->
  <struct name="FileTable">
    <field name="header" type="FileHeader" />
    <field name="entries" type="FileEntry[numFiles]" />  <!-- 使用 header 中缓存的数量 -->
  </struct>
  
  <!-- 使用表引用的结构体 -->
  <struct name="TextureInfo">
    <field name="offset" type="uint32">
      <cache>texOffset</cache>
    </field>
    <field name="width" type="uint16">
      <cache>texWidth</cache>
    </field>
    <field name="height" type="uint16">
      <cache>texHeight</cache>
    </field>
    <field name="pixelData" type="uint32@texOffset(texWidth * texHeight)">
      <naming>texture_${mule.data.sheet.index}</naming>
    </field>
  </struct>
</def>
```

#### 使用 Lua 直接定义结构

除了 XML 文件，也可以在 Lua 脚本中直接定义简单结构：

```lua
-- 定义简单结构
define("Point", {
    "x:float",
    "y:float"
})

-- 定义包含数组的结构
define("Polygon", {
    "pointCount:uint32",
    "points:Point[10]"  -- 固定 10 个点
})
```

**注意事项**：
1. Lua 定义不支持元信息
2. 复杂的结构建议使用 XML 定义
3. Lua 定义的结构不会持久化

#### 字节序

Mule 默认使用小端序（Little Endian）。如需使用大端序，在打开流时指定：

```lua
local stream = open("target:/file.dat", "rb")  -- 'b' 表示大端序
```

#### 对齐

某些平台要求数据对齐。可以使用 `dum` 类型跳过填充字节：

```xml
<struct name="AlignedData">
  <field name="flag" type="uint8" />
  <field name="padding1" type="dum8" />  <!-- 跳过 1 字节 -->
  <field name="padding2" type="dum8" />  <!-- 跳过 1 字节 -->
  <field name="padding3" type="dum8" />  <!-- 跳过 1 字节 -->
  <field name="value" type="uint32" />  <!-- 现在对齐到 4 字节边界 -->
</struct>
```

或者使用 `dum32` 一次跳过 4 字节：

```xml
<struct name="AlignedData">
  <field name="flag" type="uint8" />
  <field name="padding" type="dum8" />
  <field name="value" type="uint16" />
  <field name="align" type="dum32" />  <!-- 跳过 4 字节 -->
  <field name="data" type="uint64" />
</struct>
```

#### 错误处理

**常见错误**：

1. **类型未定义**：
   ```
   Invalid type: SomeType in definition of type MyStruct
   ```
   解决：确保类型已定义或包含了正确的定义文件

2. **XML 格式错误**：
   ```
   Failed to parse definition mydef.
   ```
   解决：检查 XML 语法，确保标签正确闭合

3. **版本不匹配**：
   ```
   Specified XML definition version is grater than support!
   ```
   解决：使用支持的版本（1 或 1.1）

4. **循环依赖**：
   结构体不能直接或间接包含自身
   
   错误示例：
   ```xml
   <struct name="Node">
     <field name="data" type="uint32" />
     <field name="next" type="Node" />  <!-- 错误：循环依赖 -->
   </struct>
   ```
   
   正确做法：使用引用或指针
   ```xml
   <struct name="Node">
     <field name="data" type="uint32" />
     <field name="nextPtr" type="uint32^" />  <!-- 使用智能引用 -->
   </struct>
   ```

### 执行细节
- 总是先执行 `config.lua`，再执行 `<action>.lua`。
- 日志级别与调试方式由 `-l` 与 `--log-file` 控制。

> TODO: 补充实际运行时行为与默认值。

## C++ 插件 API

Mule 支持通过 C++ 插件扩展功能。插件可以提供自定义的数据类型、文件容器、流转译器、文件处理器等。

### 插件基础

#### 插件结构

每个插件必须导出一个 `GetDescription` 函数，返回 `PluginDescription` 结构体指针：

```cpp
#include <Mule.h>

#ifdef WIN32
#define MYPLUGIN_API __declspec(dllexport)
#else
#define MYPLUGIN_API
#endif

extern mule::PluginDescription desc;

extern "C"
{
    mule::PluginDescription MYPLUGIN_API *GetDescription()
    {
        return &desc;
    }
}
```

#### PluginDescription 结构

```cpp
struct PluginDescription
{
    const wchar_t *name;              // 插件名称
    const wchar_t *author;            // 作者
    const wchar_t *licence;           // 许可证
    const int majorVer;               // 主版本号
    const int minorVer;               // 次版本号
    const wchar_t *description;       // 描述
    const int muleRtMajorVersion;     // 目标 MuleRt 主版本
    const int muleRtMinorVersion;     // 目标 MuleRt 次版本

    // 功能函数指针
    TypeCreator *(*GetCreators)();
    Type::DataHandler *(*CreateDataHandler)(const char16_t *name);
    Type::FileHandler *(*CreateFileHandler)(const char16_t *name);
    Stream *(*ApplyStream)(const char16_t *name, Stream *infraStream);
    FileContainer *(*ApplyContainer)(const char16_t *name, Stream *infraStream);
    Stream *(*OpenStream)(int count, const char16_t **parameters);
    FileContainer *(*OpenContainer)(int count, const char16_t **parameters);
    int (*ConvertToText)(const char16_t *converter, Stream *input, TextStream *output, const char16_t *param);
    int (*ConvertToBinary)(const char16_t *converter, TextStream *input, Stream *output, const char16_t *param);
};
```

#### 插件加载

插件在 Lua 中通过 `lplugin` 加载：

```lua
-- 加载插件
lplugin("myplugin")  -- 在 Linux 上加载 libmyplugin.so，Windows 上加载 myplugin.dll
```

#### 头文件位置

插件开发需要包含以下头文件（位于 `mulert/` 目录）：

- `<Mule.h>` - 主插件接口
- `<Data/TypeCreator.h>` - 类型创建器基类
- `<Data/Basic/Type.h>` - 数据类型基类
- `<FileContainer.h>` - 文件容器接口
- `<Stream.h>` - 流接口
- `<TextStream.h>` - 文本流接口
- `<MuleRtVersion.h>` - 运行时版本信息

### 插件类型

Mule 支持以下几种插件类型：

1. **数据类型插件**：扩展新的数据类型
2. **文件容器插件**：处理新的文件包格式
3. **流转译器插件**：处理压缩、加密等
4. **数据处理器插件**：自定义导入/导出格式
5. **文件转换器插件**：文件格式转换

### 版本兼容性

插件必须指定目标 MuleRt 版本。Mule 会检查版本兼容性：

```cpp
mule::PluginDescription desc = {
    L"MyPlugin",
    L"Author Name",
    L"MIT",
    1, 0,
    L"Plugin description",
    MULERT_MAJOR_VERSION,  // 使用宏确保版本正确
    MULERT_MINOR_VERSION,
    // ... 其他字段
};
```

如果版本不匹配，插件加载将失败并抛出异常。

### CMake 配置示例

```cmake
cmake_minimum_required(VERSION 3.12)
project(MyPlugin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含 Mule 头文件
include_directories(${MULE_INCLUDE_DIRS})

# 创建共享库
add_library(myplugin SHARED
    plugin.cpp
    # 其他源文件
)

# 链接 MuleRt
target_link_libraries(myplugin mulert)

# Windows 特定设置
if(WIN32)
    set_target_properties(myplugin PROPERTIES SUFFIX ".dll")
endif()

# Linux 特定设置
if(UNIX)
    set_target_properties(myplugin PROPERTIES PREFIX "lib")
    set_target_properties(myplugin PROPERTIES SUFFIX ".so")
endif()
```

### C++ 插件示例

#### 处理数据类型的插件示例

**场景**：实现一个自定义字符串类型，使用 `int32` 标记长度，内容对齐到 4 字节。

**实现步骤**：

1. **创建类型类**：

```cpp
// AlignedString.h
#pragma once
#include <Data/Basic/BasicType.h>

class AlignedString : public mule::Data::Basic::BasicType
{
    std::string value;
public:
    virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
    virtual void Write(xybase::Stream *stream, FileHandler *fileHandler) override;
    virtual size_t Size() const override;
    virtual std::u16string GetDataType() const override;
};
```

```cpp
// AlignedString.cpp
#include "AlignedString.h"
#include <xystring.h>

void AlignedString::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
    // 读取长度
    int32_t length = stream->ReadInt32();
    
    // 读取字符串内容
    value.resize(length);
    stream->ReadBytes(value.data(), length);
    
    // 跳过对齐填充
    size_t padding = (4 - (length % 4)) % 4;
    stream->Seek(padding, xybase::Stream::SM_CURRENT);
    
    // 通知数据处理器
    dataHandler->OnDataRead(mule::Data::Basic::MultiValue(xybase::string::to_utf16(value)));
}

void AlignedString::Write(xybase::Stream *stream, FileHandler *fileHandler)
{
    // 从文件处理器获取数据
    auto data = fileHandler->OnDataWrite();
    value = xybase::string::to_string(data.ToString());
    
    // 写入长度
    stream->Write(static_cast<int32_t>(value.length()));
    
    // 写入字符串内容
    stream->Write(value.data(), value.length());
    
    // 写入对齐填充
    size_t padding = (4 - (value.length() % 4)) % 4;
    for (size_t i = 0; i < padding; ++i)
    {
        stream->Write(static_cast<uint8_t>(0));
    }
}

size_t AlignedString::Size() const
{
    return sizeof(int32_t) + ((value.length() + 3) / 4) * 4;
}

std::u16string AlignedString::GetDataType() const
{
    return u"aligned_string";
}
```

2. **创建类型创建器**：

```cpp
// AlignedStringCreator.h
#pragma once
#include <Data/TypeCreator.h>

class AlignedStringCreator : public mule::Data::TypeCreator
{
public:
    virtual mule::Data::Basic::Type *DoCreateObject(const std::u16string &info) override;
};
```

```cpp
// AlignedStringCreator.cpp
#include "AlignedStringCreator.h"
#include "AlignedString.h"

mule::Data::Basic::Type *AlignedStringCreator::DoCreateObject(const std::u16string &info)
{
    if (info == u"aligned_string" || info == u"astring")
    {
        return new AlignedString();
    }
    return nullptr;  // 不处理，传递给下一个创建器
}
```

3. **注册到插件**：

```cpp
// plugin.cpp
#include <Mule.h>
#include "AlignedStringCreator.h"

mule::Data::TypeCreator *GetCreators()
{
    return new AlignedStringCreator();
}

mule::PluginDescription desc = {
    L"AlignedString Plugin",
    L"Your Name",
    L"MIT",
    1, 0,
    L"Provides aligned string type",
    MULERT_MAJOR_VERSION,
    MULERT_MINOR_VERSION,
    GetCreators,  // 类型创建器
    nullptr,      // DataHandler
    nullptr,      // FileHandler
    nullptr,      // ApplyStream
    nullptr,      // ApplyContainer
    nullptr,      // OpenStream
    nullptr,      // OpenContainer
    nullptr,      // ConvertToText
    nullptr       // ConvertToBinary
};

extern "C"
{
#ifdef WIN32
    __declspec(dllexport)
#endif
    mule::PluginDescription *GetDescription()
    {
        return &desc;
    }
}
```

**使用**：

```xml
<!-- 在 XML 定义中使用 -->
<def version="1.1">
  <struct name="TextData">
    <field name="text" type="aligned_string" />
  </struct>
</def>
```

或在 Lua 中：

```lua
lplugin("aligned_string")
define("TextData", {"text:aligned_string"})
```

#### 处理文件容器的插件示例

**场景**：实现 `.PAC` 文件包支持，包含文件索引表和数据区。

**文件格式**：
```
[Header: 16 bytes]
  - Magic: "PAC\0" (4 bytes)
  - Version: uint32
  - FileCount: uint32
  - DataOffset: uint32

[Index Table]
  - Entry 1: {NameOffset: uint32, DataOffset: uint32, Size: uint32}
  - Entry 2: ...
  
[String Table]
  - Null-terminated strings

[Data Area]
  - File data
```

**实现**：

```cpp
// PacContainer.h
#pragma once
#include <FileContainerBasic.h>
#include <map>

class PacContainer : public xybase::FileContainerBasic
{
    struct Header
    {
        char magic[4];
        uint32_t version;
        uint32_t fileCount;
        uint32_t dataOffset;
    };

    void ParseIndex();
    void WriteIndex();

public:
    PacContainer(xybase::Stream *stream);
    ~PacContainer();
};
```

```cpp
// PacContainer.cpp
#include "PacContainer.h"
#include <xystring.h>

PacContainer::PacContainer(xybase::Stream *stream)
    : FileContainerBasic(stream)
{
    ParseIndex();
}

PacContainer::~PacContainer()
{
    WriteIndex();  // 保存修改
}

void PacContainer::ParseIndex()
{
    infraStream->Seek(0, xybase::Stream::SM_BEGIN);
    
    // 读取头部
    Header header;
    infraStream->ReadBytes(reinterpret_cast<char*>(&header), sizeof(Header));
    
    // 验证魔数
    if (memcmp(header.magic, "PAC", 3) != 0)
    {
        throw xybase::RuntimeException(L"Invalid PAC file", 10001);
    }
    
    // 读取文件索引
    struct IndexEntry
    {
        uint32_t nameOffset;
        uint32_t dataOffset;
        uint32_t size;
    };
    
    for (uint32_t i = 0; i < header.fileCount; ++i)
    {
        IndexEntry entry;
        infraStream->ReadBytes(reinterpret_cast<char*>(&entry), sizeof(IndexEntry));
        
        // 读取文件名
        size_t currentPos = infraStream->Tell();
        infraStream->Seek(entry.nameOffset, xybase::Stream::SM_BEGIN);
        
        std::string fileName;
        char ch;
        while ((ch = infraStream->ReadInt8()) != 0)
        {
            fileName += ch;
        }
        
        infraStream->Seek(currentPos, xybase::Stream::SM_BEGIN);
        
        // 注册文件
        FileEntry *fe = new FileEntry{
            .offset = entry.dataOffset,
            .size = entry.size,
            .path = xybase::string::to_utf16(fileName),
            .occupied = false
        };
        files[fe->path] = fe;
    }
    
    // 注册空闲空间（简化版本）
    freeSpaces.RegisterFragment(header.dataOffset + CalcDataSize(), 
                                 GetFileSize() - header.dataOffset - CalcDataSize());
}

void PacContainer::WriteIndex()
{
    // 保存索引表（简化版本）
    infraStream->Seek(0, xybase::Stream::SM_BEGIN);
    
    Header header;
    memcpy(header.magic, "PAC", 4);
    header.version = 1;
    header.fileCount = static_cast<uint32_t>(files.size());
    header.dataOffset = sizeof(Header) + files.size() * 12 + CalcStringTableSize();
    
    infraStream->Write(reinterpret_cast<const char*>(&header), sizeof(Header));
    
    // 写入索引和字符串表
    uint32_t stringOffset = sizeof(Header) + files.size() * 12;
    for (auto &pair : files)
    {
        std::string name = xybase::string::to_string(pair.second->path);
        
        infraStream->Write(stringOffset);
        infraStream->Write(static_cast<uint32_t>(pair.second->offset));
        infraStream->Write(static_cast<uint32_t>(pair.second->size));
        
        stringOffset += name.length() + 1;
    }
    
    // 写入字符串表
    for (auto &pair : files)
    {
        std::string name = xybase::string::to_string(pair.second->path);
        infraStream->Write(name.c_str(), name.length() + 1);
    }
    
    infraStream->Flush();
}
```

**注册插件**：

```cpp
xybase::FileContainer *ApplyPacContainer(const char16_t *name, xybase::Stream *infraStream)
{
    if (std::u16string(name) == u"pac")
    {
        return new PacContainer(infraStream);
    }
    return nullptr;
}

mule::PluginDescription desc = {
    L"PAC Container Plugin",
    L"Your Name",
    L"MIT",
    1, 0,
    L"Supports .PAC file format",
    MULERT_MAJOR_VERSION,
    MULERT_MINOR_VERSION,
    nullptr,            // GetCreators
    nullptr,            // CreateDataHandler
    nullptr,            // CreateFileHandler
    nullptr,            // ApplyStream
    ApplyPacContainer,  // ApplyContainer
    nullptr,            // OpenStream
    nullptr,            // OpenContainer
    nullptr,            // ConvertToText
    nullptr             // ConvertToBinary
};
```

**使用**：

```lua
lplugin("pac")

-- 挂载 PAC 文件
mount(0, "pac", "data")

-- 访问文件
local file = open("data:/config.txt", "r")

-- 下略
```

#### 处理压缩文件的插件示例

**场景**：实现 ZLIB 压缩流支持。

**实现**：

```cpp
// ZlibStream.h
#pragma once
#include <StreamBasic.h>
#include <zlib.h>

class ZlibStream : public xybase::StreamBasic
{
    xybase::Stream *infraStream;
    z_stream zstream;
    std::vector<uint8_t> buffer;
    size_t virtualPos;
    size_t decompressedSize;
    
public:
    ZlibStream(xybase::Stream *infra);
    ~ZlibStream();
    
    void ReadBytes(char *buffer, size_t limit) override;
    void Write(const char *buffer, size_t size) override;
    size_t Tell() const override;
    void Seek(long long offset, SeekMode mode) override;
    void Close() override;
    void Flush() override;
    std::u16string GetName() const override;
    bool IsEof() const noexcept override;
};
```

```cpp
// ZlibStream.cpp
#include "ZlibStream.h"
#include <xystring.h>

ZlibStream::ZlibStream(xybase::Stream *infra)
    : infraStream(infra), virtualPos(0)
{
    // 初始化 zlib
    memset(&zstream, 0, sizeof(z_stream));
    inflateInit(&zstream);
    
    // 读取压缩数据
    infra->Seek(0, xybase::Stream::SM_END);
    size_t compressedSize = infra->Tell();
    infra->Seek(0, xybase::Stream::SM_BEGIN);
    
    std::vector<uint8_t> compressed(compressedSize);
    infra->ReadBytes(reinterpret_cast<char*>(compressed.data()), compressedSize);
    
    // 解压缩
    buffer.resize(compressedSize * 10);  // 估算解压后大小
    zstream.avail_in = compressedSize;
    zstream.next_in = compressed.data();
    zstream.avail_out = buffer.size();
    zstream.next_out = buffer.data();
    
    int ret = inflate(&zstream, Z_FINISH);
    if (ret != Z_STREAM_END)
    {
        throw xybase::RuntimeException(L"Decompression failed", ret);
    }
    
    decompressedSize = buffer.size() - zstream.avail_out;
    buffer.resize(decompressedSize);
}

ZlibStream::~ZlibStream()
{
    inflateEnd(&zstream);
}

void ZlibStream::ReadBytes(char *buf, size_t limit)
{
    if (virtualPos + limit > decompressedSize)
    {
        throw xybase::OutOfRangeException(L"Read beyond end of stream", 10002);
    }
    
    memcpy(buf, buffer.data() + virtualPos, limit);
    virtualPos += limit;
}

void ZlibStream::Write(const char *buf, size_t size)
{
    // 不支持写入压缩流
    throw xybase::NotImplementedException();
}

size_t ZlibStream::Tell() const
{
    return virtualPos;
}

void ZlibStream::Seek(long long offset, SeekMode mode)
{
    switch (mode)
    {
    case SM_BEGIN:
        virtualPos = offset;
        break;
    case SM_CURRENT:
        virtualPos += offset;
        break;
    case SM_END:
        virtualPos = decompressedSize + offset;
        break;
    }
    
    if (virtualPos > decompressedSize)
    {
        throw xybase::OutOfRangeException(L"Seek beyond end", 10003);
    }
}

void ZlibStream::Close()
{
    // 清理资源
}

void ZlibStream::Flush()
{
    // 只读流，无需刷新
}

std::u16string ZlibStream::GetName() const
{
    return infraStream->GetName() + u"|zlib";
}

bool ZlibStream::IsEof() const noexcept
{
    return virtualPos >= decompressedSize;
}
```

**注册插件**：

```cpp
xybase::Stream *ApplyZlibStream(const char16_t *name, xybase::Stream *infraStream)
{
    if (std::u16string(name) == u"zlib")
    {
        return new ZlibStream(infraStream);
    }
    return nullptr;
}

mule::PluginDescription desc = {
    L"ZLIB Stream Plugin",
    L"Your Name",
    L"MIT",
    1, 0,
    L"Supports ZLIB compression",
    MULERT_MAJOR_VERSION,
    MULERT_MINOR_VERSION,
    nullptr,          // GetCreators
    nullptr,          // CreateDataHandler
    nullptr,          // CreateFileHandler
    ApplyZlibStream,  // ApplyStream
    nullptr,          // ApplyContainer
    nullptr,          // OpenStream
    nullptr,          // OpenContainer
    nullptr,          // ConvertToText
    nullptr           // ConvertToBinary
};
```

**使用**：

```lua
lplugin("zlib")

-- 打开压缩文件
local compressed = open("target:/data.zlib", "r")
local decompressed = sos(compressed, "zlib")

-- 读取解压后的数据
local data = read(decompressed, 1024)
```

#### 复杂文件容器的插件示例

**场景**：处理 `.PAC` + `.TOC` 分离的文件包。

**实现使用 OpenContainer**：

```cpp
xybase::FileContainer *OpenPacContainer(int count, const char16_t **parameters)
{
    // 参数格式："pac" "path/to/file.pac" "path/to/file.toc"
    if (count < 2 || std::u16string(parameters[0]) != u"pac")
    {
        return nullptr;
    }
    
    std::u16string pacPath = parameters[1];
    std::u16string tocPath = count >= 3 ? parameters[2] : (pacPath + u".toc");
    
    // 打开 PAC 文件
    auto pacStream = mule::VirtualFileSystem::GetInstance().Open(pacPath.c_str(), xybase::FOM_READ | xybase::FOM_WRITE);
    if (!pacStream)
    {
        return nullptr;
    }
    
    // 创建容器
    auto container = new PacContainer(pacStream);
    
    // 如果提供了 TOC 文件，加载索引
    if (count >= 3)
    {
        auto tocStream = mule::VirtualFileSystem::GetInstance().Open(tocPath.c_str(), xybase::FOM_READ);
        if (tocStream)
        {
            container->LoadIndexFromToc(tocStream);
            delete tocStream;
        }
    }
    
    return container;
}

mule::PluginDescription desc = {
    // ...
    nullptr,            // OpenStream
    OpenPacContainer,   // OpenContainer
    // ...
};
```

**使用**：

```lua
lplugin("pac_toc")

-- 使用 openpack 打开
openpack("data", "pac target:/game.pac target:/game.toc")

-- 访问文件
local file = open("data:/config.bin", "r")
```

#### 文件格式转换器插件示例

**场景**：实现一个自定义文件格式 `.FOO` 与文本格式（如 JSON）之间的相互转换。这在想要完全自定义文件格式时非常有用。

**文件格式说明**（`.FOO` 格式）：
```
[Header: 8 bytes]
  - Magic: "FOO\0" (4 bytes)
  - Version: uint32

[Data Section]
  - RecordCount: uint32
  - Records: [{id: uint32, name: string(null-terminated), value: float}...]
```

**目标文本格式**（JSON）：
```json
{
  "version": 1,
  "records": [
    {"id": 1, "name": "item1", "value": 3.14},
    {"id": 2, "name": "item2", "value": 2.71}
  ]
}
```

**实现步骤**：

1. **创建转换器类**：

```cpp
// FooConverter.h
#pragma once
#include <Stream.h>
#include <TextStream.h>
#include <string>
#include <vector>

struct FooRecord
{
    uint32_t id;
    std::string name;
    float value;
};

class FooConverter
{
public:
    // 二进制转文本
    static int ConvertToText(const char16_t *converter, 
                            xybase::Stream *input, 
                            xybase::TextStream *output, 
                            const char16_t *param);
    
    // 文本转二进制
    static int ConvertToBinary(const char16_t *converter, 
                              xybase::TextStream *input, 
                              xybase::Stream *output, 
                              const char16_t *param);

private:
    static bool ReadFooFile(xybase::Stream *input, uint32_t &version, std::vector<FooRecord> &records);
    static void WriteFooFile(xybase::Stream *output, uint32_t version, const std::vector<FooRecord> &records);
    static void WriteJson(xybase::TextStream *output, uint32_t version, const std::vector<FooRecord> &records);
    static bool ReadJson(xybase::TextStream *input, uint32_t &version, std::vector<FooRecord> &records);
};
```

2. **实现转换逻辑**：

```cpp
// FooConverter.cpp
#include "FooConverter.h"
#include <xystring.h>
#include <sstream>

int FooConverter::ConvertToText(const char16_t *converter, 
                                xybase::Stream *input, 
                                xybase::TextStream *output, 
                                const char16_t *param)
{
    // 检查转换器名称
    if (std::u16string(converter) != u"foo")
    {
        return -1;  // 不处理，传递给下一个转换器
    }
    
    // 读取二进制数据
    uint32_t version;
    std::vector<FooRecord> records;
    
    if (!ReadFooFile(input, version, records))
    {
        throw xybase::RuntimeException(L"Failed to read FOO file", 10100);
    }
    
    // 写入文本（JSON 格式）
    WriteJson(output, version, records);
    
    return 0;  // 成功处理
}

int FooConverter::ConvertToBinary(const char16_t *converter, 
                                 xybase::TextStream *input, 
                                 xybase::Stream *output, 
                                 const char16_t *param)
{
    // 检查转换器名称
    if (std::u16string(converter) != u"foo")
    {
        return -1;  // 不处理
    }
    
    // 读取文本（JSON 格式）
    uint32_t version;
    std::vector<FooRecord> records;
    
    if (!ReadJson(input, version, records))
    {
        throw xybase::RuntimeException(L"Failed to read JSON", 10101);
    }
    
    // 写入二进制
    WriteFooFile(output, version, records);
    
    return 0;  // 成功处理
}

bool FooConverter::ReadFooFile(xybase::Stream *input, uint32_t &version, std::vector<FooRecord> &records)
{
    input->Seek(0, xybase::Stream::SM_BEGIN);
    
    // 读取并验证头部
    char magic[4];
    input->ReadBytes(magic, 4);
    if (memcmp(magic, "FOO", 3) != 0)
    {
        return false;
    }
    
    version = input->ReadUInt32();
    
    // 读取记录
    uint32_t recordCount = input->ReadUInt32();
    records.reserve(recordCount);
    
    for (uint32_t i = 0; i < recordCount; ++i)
    {
        FooRecord rec;
        rec.id = input->ReadUInt32();
        
        // 读取以 null 结尾的字符串
        char ch;
        while ((ch = input->ReadInt8()) != 0)
        {
            rec.name += ch;
        }
        
        rec.value = input->ReadFloat();
        records.push_back(rec);
    }
    
    return true;
}

void FooConverter::WriteFooFile(xybase::Stream *output, uint32_t version, const std::vector<FooRecord> &records)
{
    output->Seek(0, xybase::Stream::SM_BEGIN);
    
    // 写入头部
    output->Write("FOO", 4);
    output->Write(version);
    
    // 写入记录
    output->Write(static_cast<uint32_t>(records.size()));
    
    for (const auto &rec : records)
    {
        output->Write(rec.id);
        output->Write(rec.name.c_str(), rec.name.length() + 1);  // 包含 null 终止符
        output->Write(rec.value);
    }
}

void FooConverter::WriteJson(xybase::TextStream *output, uint32_t version, const std::vector<FooRecord> &records)
{
    output->Write("{\n");
    output->Write("  \"version\": ");
    output->Write(version);
    output->Write(",\n");
    output->Write("  \"records\": [\n");
    
    for (size_t i = 0; i < records.size(); ++i)
    {
        const auto &rec = records[i];
        
        output->Write("    {\"id\": ");
        output->Write(rec.id);
        output->Write(", \"name\": \"");
        output->Write(rec.name);
        output->Write("\", \"value\": ");
        output->Write(rec.value);
        output->Write("}");
        
        if (i < records.size() - 1)
        {
            output->Write(",\n");
        }
        else
        {
            output->Write("\n");
        }
    }
    
    output->Write("  ]\n");
    output->Write("}\n");
}

bool FooConverter::ReadJson(xybase::TextStream *input, uint32_t &version, std::vector<FooRecord> &records)
{
    // 解析 JSON，略
}
```

3. **注册到插件**：

```cpp
// plugin.cpp
#include <Mule.h>
#include "FooConverter.h"

mule::PluginDescription desc = {
    L"FOO Format Converter",
    L"Your Name",
    L"MIT",
    1, 0,
    L"Converts between .FOO binary format and JSON text",
    MULERT_MAJOR_VERSION,
    MULERT_MINOR_VERSION,
    nullptr,                            // GetCreators
    nullptr,                            // CreateDataHandler
    nullptr,                            // CreateFileHandler
    nullptr,                            // ApplyStream
    nullptr,                            // ApplyContainer
    nullptr,                            // OpenStream
    nullptr,                            // OpenContainer
    FooConverter::ConvertToText,        // ConvertToText
    FooConverter::ConvertToBinary       // ConvertToBinary
};

extern "C"
{
#ifdef WIN32
    __declspec(dllexport)
#endif
    mule::PluginDescription *GetDescription()
    {
        return &desc;
    }
}
```

**使用示例**：

```lua
lplugin("foo")

-- 二进制转文本
cvttxt(0, "foo", "output.json", "")
-- 将流 0 的内容转换为 JSON，输出到 resources/output.json

-- 文本转二进制
cvtbin("output.json", "foo", 0, "")
-- 将 resources/output.json 转换为二进制，写入流 0
```

**带参数的转换器示例**：

转换器可以通过参数字符串接收额外配置：

```cpp
int FooConverter::ConvertToText(const char16_t *converter, 
                                xybase::Stream *input, 
                                xybase::TextStream *output, 
                                const char16_t *param)
{
    if (std::u16string(converter) != u"foo")
    {
        return -1;
    }
    
    uint32_t version;
    std::vector<FooRecord> records;
    
    if (!ReadFooFile(input, version, records))
    {
        throw xybase::RuntimeException(L"Failed to read FOO file", 10100);
    }
    
    // 解析参数
    std::u16string paramStr(param);
    bool prettyPrint = paramStr.find(u"pretty") != std::u16string::npos;
    bool includeMetadata = paramStr.find(u"metadata") != std::u16string::npos;
    
    // 根据参数调整输出格式
    if (includeMetadata)
    {
        output->Write("// Generated by FOO Converter\n");
        output->Write("// Version: ");
        output->Write(version);
        output->Write("\n");
    }
    
    if (prettyPrint)
    {
        WriteJson(output, version, records);  // 使用美化格式
    }
    else
    {
        WriteJsonCompact(output, version, records);  // 紧凑格式
    }
    
    return 0;
}
```

**使用参数**：

```lua
lplugin("foo")

-- 使用参数：美化输出并包含元数据
cvttxt(0, "foo", "output.json", "pretty metadata")

-- 紧凑输出
cvttxt(0, "foo", "compact.json", "")
```

**实际示例：advstr 插件**

Mule 内置的 `advstr` 插件提供了地址文本转换功能：

```cpp
int b2t(const char16_t *converter, xybase::Stream *input, xybase::TextStream *output, const char16_t *param)
{
    if (0 == memcmp(u"addrtxt", converter, sizeof(u"addrtxt")))
    {
        // 解析参数：起始地址:结束地址
        std::u16string para(param);
        std::u16string sta = para.substr(0, para.find(':')),
            end = para.substr(para.find(':') + 1);
        
        // 调用实际转换函数
        return str_ex(input, output, 
                     (int)xybase::string::stoi(sta, 16), 
                     (int)xybase::string::stoi(end, 16));
    }
    return -1;  // 不处理
}

int t2b(const char16_t *converter, xybase::TextStream *input, xybase::Stream *output, const char16_t *param)
{
    if (0 == memcmp(u"addrtxt", converter, sizeof(u"addrtxt")))
    {
        return str_im(input, output);
    }
    return -1;
}
```

**使用 advstr**：

```lua
lplugin("advstr")

-- 导出地址范围内的文本
cvttxt(0, "addrtxt", "strings.txt", "1000:2000")
-- 从地址 0x1000 到 0x2000 导出字符串

-- 导入文本
cvtbin("strings.txt", "addrtxt", 0, "")
```

**返回值约定**：

- **返回 0**：表示成功处理该转换请求
- **返回 -1**：表示不处理该转换器，让下一个插件尝试
- **抛出异常**：表示转换失败，中止操作

**注意事项**：

1. **转换器名称**：
   - 转换器名称应该是唯一的
   - 建议使用小写字母和数字
   - 避免与内置转换器冲突

2. **参数处理**：
   - 参数字符串可以包含任何格式的数据
   - 建议使用简单的格式（如 `key:value` 或空格分隔）
   - 需要自行解析参数字符串

3. **流状态**：
   - 转换前应将输入流定位到正确位置
   - 转换后应刷新输出流确保数据写入
   - 注意流的读写模式

4. **文本编码**：
   - `TextStream` 使用系统默认编码
   - 可以通过 locale 参数指定编码
   - 建议使用 UTF-8 编码

5. **错误处理**：
   - 使用 Mule 提供的异常类型
   - 提供详细的错误信息
   - 验证输入数据的有效性

### 插件最佳实践

1. **错误处理**：
   - 使用 Mule 提供的异常类型
   - 返回 `nullptr` 表示不处理，让下一个创建器尝试
   - 提供详细的错误信息

2. **资源管理**：
   - 在析构函数中正确清理资源
   - 使用 RAII 模式管理资源
   - 避免内存泄漏

3. **线程安全**：
   - 注意插件可能被多次调用
   - 避免使用全局可变状态
   - 使用互斥锁保护共享资源

4. **性能优化**：
   - 缓存常用数据
   - 避免不必要的内存拷贝
   - 使用合适的数据结构

5. **版本兼容性**：
   - 始终使用 `MULERT_MAJOR_VERSION` 和 `MULERT_MINOR_VERSION` 宏
   - 测试与不同版本的兼容性
   - 在文档中说明最低支持的版本

## 全局 Config 键列表
已知的配置键：
- `mule.target`：目标路径。
- `mule.target.type`：`file` 或 `directory`。
- `mule.basedir`：工作区路径。
- `mule.resource.basedir`：资源根路径。
- `mule.script.basedir`：脚本目录。
- `mule.data.basedir`：数据目录。
- `mule.sheet.basedir`：表格目录。
- `mule.data.sheet.name`：当前正在读写的表名。
- `mule.data.sheet.index`：当前正在处理的表元素索引。
- `mule.data.sheet.structure-simplify-suppression`：非 `0` 时禁用单元素表结构简化。
- `mule.sheet-reference.name-pattern`：表引用生成名称的模板。
- `mule.data.smart-reference.align`：智能引用分配的对齐值（默认为 1）。若设为大于 1 的值，在导出时，长度会自动向上对齐到该值的倍数；在导入时，分配的空间大小也会向上对齐到该值的倍数，且起始地址会对齐到该值的倍数。（必须是 2 的幂）

> TODO: 补充更多可用的全局配置键（含插件与运行时相关项）。

## LuaAPI （另外文档）
请参阅 [LuaAPI](LuaAPI.md) 文档以获取 Lua API 的详细信息。



