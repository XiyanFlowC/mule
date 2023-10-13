#pragma once

#include <FileContainer.h>
#include <string>
#include <map>
#include <list>

#include "mulert_api.h"
#include "Logger.h"

namespace mule
{
	/**
	 * @brief 管理文件容器类的管理器。文件容器用于将Stream解释为虚拟文件系统。
	*/
	class MULERT_API VirtualFileSystem
	{
		Logger logger = Logger::GetLogger<VirtualFileSystem>();

		std::map <std::u16string, xybase::FileContainer *> containers;

		std::list <xybase::FileContainer *(*)(const char16_t *, xybase::Stream *)> containerApplyers;
	public:

		static VirtualFileSystem &GetInstance();

		const std::list<std::u16string> &List(const char16_t *container);

		/**
		 * @brief 打开指定路径的流
		 * @param path 指定路径的流
		 * @param openMode 打开方式
		 * @return 打开的流
		*/
		xybase::Stream *Open(const char16_t *path, xybase::FileOpenMode openMode);

		/**
		 * @brief 挂载一个容器到指定根【此后此容器生命期需由本类管理】
		 * @param rootName 挂载的根名
		 * @param container 要挂载的容器
		*/
		void Mount(const char16_t *rootName, xybase::FileContainer *container);

		/**
		 * @brief 以指定的type创建文件容器，并挂载到指定根
		 * @param rootName 要挂载的根
		 * @param containerType 解释流的容器类型
		 * @param infraStream 要被解释的下层流
		*/
		void Mount(const char16_t *rootName, const char16_t *containerType, xybase::Stream *infraStream);

		/**
		 * @brief 解除挂载
		 * @param name 指定的根名。
		*/
		void Unmount(const char16_t *name);

		/**
		 * @brief 列出当前所有根名。
		 * @return 所有根名组成的列表
		*/
		const std::list<std::u16string> ListRoots() const;

		/**
		 * @brief 注册一个创建器到虚拟文件系统。
		 * @param creator 指向创建函数的指针
		*/
		void RegisterContainerCreator(xybase::FileContainer *(*creator)(const char16_t *type, xybase::Stream *infra));
	};
}
