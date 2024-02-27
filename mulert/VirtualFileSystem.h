#pragma once

#include <FileContainer.h>
#include <string>
#include <functional>
#include <map>
#include <list>

#include "mulert_api.h"
#include "Logger.h"

namespace mule
{
	/**
	 * @brief 管理文件容器类的管理器。文件容器用于将Stream解释为虚拟文件系统。
	*/
	class VirtualFileSystem
	{
		Logger logger = Logger::GetLogger<VirtualFileSystem>();

		std::map <std::u16string, xybase::FileContainer *> containers;

		std::list <xybase::FileContainer *(*)(const char16_t *, xybase::Stream *)> containerApplyers;
	public:

		MULERT_API static VirtualFileSystem &GetInstance();

		MULERT_API const std::list<std::u16string> List(const char16_t *container);

		/**
		 * @brief 打开指定路径的流
		 * @param path 指定路径的流
		 * @param openMode 打开方式
		 * @return 打开的流
		*/
		MULERT_API xybase::Stream *Open(const char16_t *path, xybase::FileOpenMode openMode);

		/**
		 * @brief 从文件系统中移除一个文件
		 * @param path 指定的文件路径
		 * @param recursive 递归移除子目录
		*/
		MULERT_API void Remove(const char16_t *path, bool recursive = false);

		/**
		 * @brief 级联打开文件，调用闭包后关闭
		 * @param targetFile 文件全限定路径
		 * @param openMode 打开方式
		 * @param lambda 回调闭包
		*/
		MULERT_API void CascadeProcess(const char16_t *targetFile, std::function<void(xybase::Stream *target)> lambda, xybase::FileOpenMode openMode);

		/**
		 * @brief 挂载一个容器到指定根【此后此容器生命期需由本类管理】
		 * @param rootName 挂载的根名
		 * @param container 要挂载的容器
		*/
		MULERT_API void Mount(const char16_t *rootName, xybase::FileContainer *container);

		/**
		 * @brief 以指定的type创建文件容器，并挂载到指定根
		 * @param rootName 要挂载的根
		 * @param containerType 解释流的容器类型
		 * @param infraStream 要被解释的下层流
		*/
		MULERT_API void Mount(const char16_t *rootName, const char16_t *containerType, xybase::Stream *infraStream);

		/**
		 * @brief 解除挂载
		 * @param name 指定的根名。
		*/
		MULERT_API void Unmount(const char16_t *name);

		/**
		 * @brief 列出当前所有根名。
		 * @return 所有根名组成的列表
		*/
		MULERT_API const std::list<std::u16string> ListRoots() const;

		/**
		 * @brief 注册一个创建器到虚拟文件系统。
		 * @param creator 指向创建函数的指针
		*/
		MULERT_API void RegisterContainerCreator(xybase::FileContainer *(*creator)(const char16_t *type, xybase::Stream *infra));
	};
}
