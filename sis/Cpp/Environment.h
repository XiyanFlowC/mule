#pragma once
#ifndef ENVIRONMENT_H__
#define ENVIRONMENT_H__

#include <string>
#include <Stream.h>
#include <FileContainer.h>
#include <Exception/InvalidParameterException.h>
#include <Data/TypeCreator.h>
#include <Data/Basic/MultiValue.h>
#include <Data/Basic/Type.h>
#include <Data/TypeManager.h>

/* For plugin development */
namespace mule
{
	namespace Cpp
	{
		/**
		 * @brief 适应环境的插件描述
		*/
		struct PluginDescription
		{
			const char *name;
			const char *author;
			const char *licence;
			const int majorVer;
			const int minorVer;
			const char *description;
			mule::Data::TypeCreator *(*GetCreators)();
			mule::Data::Basic::Type::DataHandler *(*GetHandler)(const char16_t *name);
			xybase::Stream *(*GetStream)(const char16_t *name);
			xybase::Stream *(*ApplyStream)(const char16_t *name, xybase::Stream *infraStream);
			xybase::FileContainer *(*GetFileContainer)(const char16_t *name, xybase::Stream *infraStream);
		};

		/**
		 * @brief 单例类。环境，用于表示Cpp下的操作环境。提供插件加载功能。
		*/
		class Environment
		{
			std::map<std::string, void *> handlers;

			std::list<PluginDescription *> descriptions;

			Environment();

			~Environment();

			void *GetFunction(std::string moduleName, std::string functionName);
		public:
			static Environment &GetInstance();

			/**
			 * @brief 打开指定的插件（dll）。并将其包含的功能注册到环境。
			 * @param moduleName 要分配的插件名
			 * @param path 插件的路径
			*/
			void OpenPlugin(std::string moduleName, std::string path);

			/**
			 * @brief 加载指定的描述到环境。
			 * @param desc 插件描述
			*/
			void LoadDescription(mule::Cpp::PluginDescription *desc);

			/**
			 * @brief 关闭指定的插件（不得使用，尚未实现在TypeManager解除注册）
			 * @param moduleName 插件名
			*/
			void ClosePlugin(std::string moduleName);

			const std::list<PluginDescription *> &GetDescriptions();

			mule::Data::Basic::Type::DataHandler *GetHandler(const std::u16string &name);

			xybase::Stream *GetStream(std::u16string name);

			xybase::Stream *ApplyStream(std::u16string name, xybase::Stream *infraStream);

			xybase::FileContainer *GetFileContainer(std::u16string name, xybase::Stream *infraStream);
		};
	}
}

#endif
