# MULE
骡子：设计为将二进制文件转换为文本文件或其逆动作的软件。

## MULERT （MULE 运行时）
Mule Runtime，为动态库。设计用于提供基本的类型，并允许插件链接从而允许不修改主程序的二次开发。

若不需要插件功能则可以使用其静态链接版本。

## BIS （基本实现系统）
Basic Implementation System 提供一套基本类型的读写功能实现。包括整型、浮点型和零结尾字符串以及指针。

## SIS （系统交互系统）
System Interaction System 提供数个和其他语言的交互实现。

## MULE （主程序）
MULE 本身是用于实验的，也提供了一个能够通过脚本环境完成操作的软件。

MULE 要求指定目标文件和工作区。工作区中包括data, resources, sheets三个文件夹。

* resources/definitions：用于存放定义，软件最终将从这里读取定义。
* resources/scripts：用于存放脚本，软件将从这里执行脚本。
* sheets：用于存放抽取（或需要导入的）的文本数据。
* data：用于存放抽取（或需要导入的）二进制数据。
