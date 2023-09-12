#pragma once

#ifndef XY_FILE_CONTAINER_H__
#define XY_FILE_CONTAINER_H__

#include <list>

#include "Stream.h"

namespace xybase
{
	using FileOpenMode = int;

	const FileOpenMode
		FOM_READ = 0x1,
		FOM_WRITE = 0x2,
		FOM_TRUNCATE = 0x10,
		FOM_APPEND = 0x20,
		FOM_EXCLUSIVE = 0x40;

	/**
	 * @brief 文件容器。抽象类，用于表示虚拟文件系统/文件包。
	*/
	class XY_API FileContainer
	{
	public:
		virtual ~FileContainer();

		/**
		 * @brief 获取容器名称，该名称应唯一地标识此实例。
		 * @return 容器名称。
		*/
		virtual std::u16string GetName() = 0;

		/**
		 * @brief 刷新容器缓冲区（文件包用，其他类型保持空操作）。
		*/
		virtual void Flush() = 0;

		/**
		 * @brief 以指定模式打开一个文件用于读写。
		 * @param name 文件名。
		 * @param mode 打开模式。
		 * @return 打开的文件流。
		*/
		virtual xybase::Stream *Open(std::u16string name, FileOpenMode mode = FOM_READ | FOM_WRITE) = 0;

		/**
		 * @brief 列出所有文件。
		 * @return 文件列表。
		*/
		virtual std::list<std::u16string> List() = 0;

		/**
		 * @brief 创建目录。
		 * @param path 指定路径。
		*/
		virtual void MakeDir(std::u16string path) = 0;

		/**
		 * @brief 移除一个文件或目录。
		 * @param path 文件或目录的路径。
		 * @param recursive 是否递归删除子项（目录有效，文件忽略此参数）。
		*/
		virtual void Remove(std::u16string path, bool recursive = false) = 0;
	};
}

#endif
