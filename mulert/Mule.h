#pragma once

#include <FileContainer.h>
#include <TextStream.h>
#include <cstdint>
#include "mulert_api.h"

#include "Storage/BinaryData.h"
#include "Data/TypeCreator.h"
#include "Logger.h"

namespace mule
{
	/**
	 * @brief Mule插件描述符
	*/
	struct PluginDescription
	{
		/**
		 * @brief 插件的名称
		*/
		const wchar_t *name;
		/**
		 * @brief 插件的作者
		*/
		const wchar_t *author;
		/**
		 * @brief 插件的授权协议
		*/
		const wchar_t *licence;
		/**
		 * @brief 主要版本
		*/
		const int majorVer;
		/**
		 * @brief 次要版本
		*/
		const int minorVer;
		/**
		 * @brief 插件自述
		*/
		const wchar_t *description;

		// 插件的功能函数暴露注册，nullptr指定没有相应接口
		// 结构解析/文本化反文本化操作
		/**
		 * @brief 获取插件中包括的全部创建器的链
		*/
		mule::Data::TypeCreator *(*GetCreators)();
		/**
		 * @brief 获取指定名称的数据处理器的实例（须自行管理生命周期）
		*/
		mule::Data::Basic::Type::DataHandler *(*GetDataHandler)(const char16_t *name);
		/**
		 * @brief 获取指定名称的文件处理器的实例（须自行管理生命周期）
		*/
		mule::Data::Basic::Type::FileHandler *(*GetFileHandler)(const char16_t *name);
		// 流解析/文件操作
		/**
		 * @brief 将一个下层流解释为另一个流
		*/
		xybase::Stream *(*ApplyStream)(const char16_t *name, xybase::Stream *infraStream);
		/**
		 * @brief 将一个下层流解释为指定的文件容器
		*/
		xybase::FileContainer *(*ApplyContainer)(const char16_t *name, xybase::Stream *infraStream);
		/**
		 * @brief 以给定参数打开一个流
		*/
		xybase::Stream *(*OpenStream)(int count, const char16_t **parameters);
		/**
		 * @brief 以给定参数打开一个文件容器
		*/
		xybase::FileContainer *(*OpenContainer)(int count, const char16_t **parameters);
		/**
		 * @brief 使用指定的转换器将输入流转换为文本流输出
		*/
		void (*ConvertText)(const char16_t *converter, xybase::Stream *input, xybase::TextStream *output);
		/**
		 * @brief 使用指定的转换器将文本流输入转换为二进制流输出
		*/
		void (*ConvertBinary)(const char16_t *converter, xybase::TextStream *input, xybase::Stream *output);
	};

	/**
	 * @brief 骡子大杂脍。没想好放哪里的函数都先堆在这里了。主要是插件相关的处理。
	*/
	class MULERT_API Mule
	{
		Logger logger = Logger::GetLogger<Mule>();

	public:
		virtual ~Mule();
		
		static Mule &GetInstance();

		/**
		 * @brief 导出指定文件到数据文件。
		 * @param targetFile 文件全限定路径。包括根和全部转义流名。
		 * @param id 数据文件ID。
		*/
		void Export(const char16_t *targetFile, uint32_t id);

		/**
		 * @brief 从数据文件导入到指定文件。
		 * @param targetFile 文件全限定路径。包括根和全部转义流名。
		 * @param id 数据文件ID
		*/
		void Import(const char16_t *targetFile, uint32_t id);

		/**
		 * @brief 加载指定的插件动态库。
		 * @param path 文件路径。
		*/
		void LoadPlugin(const char16_t *plugin);

		/**
		 * @brief 加载指定的描述
		 * @param description 加载指定的描述
		*/
		void LoadDescription(const PluginDescription *description);

		/**
		 * @brief 展示已载入的插件
		 * @return 可显示的字符串
		*/
		std::wstring ShowPlugins();

		/**
		 * @brief 在日志中反应已经加载的插件
		*/
		void PrintPlugins();

		/**
		 * @brief 应用一个流（转译一个流，将一个流以另一种形式处理【（解）压缩/ELF记忆体地址映射等用】）
		 * @param typeName 转译流类型
		 * @param infraStream 下层流
		 * @return 转译流对象，失败返回 nullptr
		*/
		xybase::Stream *ApplyStream(const char16_t *typeName, xybase::Stream *infraStream);

		/**
		 * @brief 挂载一个流（转译一个流，将此流视作文件包（虚拟文件系统））
		 * @param typeName 文件包类型
		 * @param infraStream 下层流
		*/
		void MountStream(const char16_t *mountName, const char16_t *typeName, xybase::Stream *infraStream);

		/**
		 * @brief 按名称获取数据处理器
		 * @param name 处理器名称
		 * @return 取得的处理器，失败返回 nullptr
		*/
		mule::Data::Basic::Type::DataHandler *GetDataHandler(const char16_t *name);

		/**
		 * @brief 按名称获取文件处理器
		 * @param name 处理器名称
		 * @return 取得的处理器，失败返回 nullptr
		*/
		mule::Data::Basic::Type::FileHandler *GetFileHandler(const char16_t *name);
	protected:
		std::list<void *> pluginHandlers;
		std::list<const PluginDescription *> descriptions;

	private:
		Mule();
	};
}