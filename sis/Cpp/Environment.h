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

		class Environment
		{
			std::map<std::string, void *> handlers;

			std::list<PluginDescription *> descriptions;

			Environment();

			~Environment();
		public:
			Environment &GetInstance();

			void OpenPlugin(std::string moduleName, std::string path);

			void LoadDescription(mule::Cpp::PluginDescription *desc);

			void *GetFunction(std::string moduleName, std::string functionName);

			void ClosePlugin(std::string moduleName);

			mule::Data::Basic::Type::DataHandler *GetHandler(const std::u16string &name);

			xybase::Stream *GetStream(std::u16string name);

			xybase::Stream *ApplyStream(std::u16string name, xybase::Stream *infraStream);

			xybase::FileContainer *GetFileContainer(std::u16string name, xybase::Stream *infraStream);
		};
	}
}

#endif
