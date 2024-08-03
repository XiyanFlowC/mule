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
		/**
		 * @brief 适应的 MuleRt 版本
		 */
		const int muleRtMajorVersion;
		/**
		 * @brief 适应的 MuleRt 次要版本
		 */
		const int muleRtMinorVersion;

		// 插件的功能函数暴露注册，nullptr指定没有相应接口
		// 结构解析/文本化反文本化操作
		/**
		 * @brief 获取插件中包括的全部创建器的实例的链
		*/
		mule::Data::TypeCreator *(*GetCreators)();
		/**
		 * @brief 创建指定名称的数据处理器的实例
		*/
		mule::Data::Basic::Type::DataHandler *(*CreateDataHandler)(const char16_t *name);
		/**
		 * @brief 创建指定名称的文件处理器的实例
		*/
		mule::Data::Basic::Type::FileHandler *(*CreateFileHandler)(const char16_t *name);
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
		int (*ConvertToText)(const char16_t *converter, xybase::Stream *input, xybase::TextStream *output, const char16_t *param);
		/**
		 * @brief 使用指定的转换器将文本流输入转换为二进制流输出
		*/
		int (*ConvertToBinary)(const char16_t *converter, xybase::TextStream *input, xybase::Stream *output, const char16_t *param);
	};

	/**
	 * @brief 骡子大杂脍。没想好放哪里的函数都先堆在这里了。主要是插件相关的处理。
	*/
	class Mule
	{
		Logger logger = Logger::GetLogger<Mule>();

	public:
		MULERT_API virtual ~Mule();
		
		MULERT_API static Mule &GetInstance();

		/**
		 * @brief 导出指定文件到数据文件。
		 * @param targetFile 文件全限定路径。包括根和全部转义流名。
		 * @param id 数据文件ID。
		*/
		MULERT_API void Export(const char16_t *targetFile, uint32_t id);

		/**
		 * @brief 从指定文件抽取数据段，保存到数据文件
		 * @param targetFile 指定的文件全限定路径。
		 * @param offset 偏移量
		 * @param length 长度
		 * @param id 数据文件 ID。
		*/
		MULERT_API void Extract(const char16_t *targetFile, size_t offset, size_t length, uint32_t id);

		/**
		 * @brief 从数据文件导入到指定文件。
		 * @param targetFile 文件全限定路径。包括根和全部转义流名。
		 * @param id 数据文件ID
		*/
		MULERT_API void Import(const char16_t *targetFile, uint32_t id);

		/**
		 * @brief 尝试从数据文件导入到指定文件。发生异常时则忽略。
		 * @param targetFile 文件全限定路径。包括根和全部转义流名。
		 * @param id 数据文件ID
		 */
		MULERT_API void TryImport(const char16_t *targetFile, uint32_t id);

		/**
		 * @brief 从数据文件获得的数据覆盖到指定位置。
		 * @param targetFile 指定的文件全限定路径。
		 * @param offset 偏移量
		 * @param length 长度
		 * @param id 数据文件 ID。
		*/
		MULERT_API void Patch(const char16_t *targetFile, size_t offset, size_t length, uint32_t id);

		/**
		 * @brief 转换文件到给定的文本
		 * @param target 目标文件在虚拟文件系统中的路径
		 * @param converter 转换器
		 * @param dest 目的位置
		 * @param param 参数字符串
		*/
		MULERT_API void ConvertToText(xybase::Stream *target, const char16_t *converter, xybase::TextStream *dest, const char16_t *param);

		/**
		 * @brief 转文本到给定的文件
		 * @param sourceText 原始文本文件名
		 * @param converter 转换器
		 * @param target 目标二进制文件在虚拟文件系统中的路径
		 * @param param 参数字符串
		*/
		MULERT_API void ConvertToBinary(xybase::TextStream *sourceText, const char16_t *converter, xybase::Stream *target, const char16_t *param);

		/**
		 * @brief 加载指定的插件动态库。
		 * @param path 文件路径。
		*/
		MULERT_API void LoadPlugin(const char16_t *plugin);

		/**
		 * @brief 加载指定的描述
		 * @param description 加载指定的描述
		*/
		MULERT_API void LoadDescription(const PluginDescription *description);

		/**
		 * @brief 展示已载入的插件
		 * @return 可显示的字符串
		 * @deprecated
		*/
		MULERT_API std::wstring ShowPlugins();

		/**
		 * @brief 应用一个流（转译一个流，将一个流以另一种形式处理【（解）压缩/ELF记忆体地址映射等用】）
		 * @param typeName 转译流类型
		 * @param infraStream 下层流
		 * @return 转译流对象，失败返回 nullptr
		*/
		MULERT_API xybase::Stream *ApplyStream(const char16_t *typeName, xybase::Stream *infraStream);

		/**
		 * @brief 挂载一个流（转译一个流，将此流视作文件包（虚拟文件系统））
		 * @param typeName 文件包类型
		 * @param infraStream 下层流
		*/
		MULERT_API void MountStream(const char16_t *mountName, const char16_t *typeName, xybase::Stream *infraStream);

		/**
		 * @brief 打开并挂载。
		 * @param root 要挂载的根。
		 * @param params 开启参数。
		*/
		MULERT_API void OpenAndMount(const char16_t *root, const char16_t *params);

		/**
		 * @brief 按名称获取数据处理器
		 * @param name 处理器名称
		 * @return 取得的处理器，失败返回 nullptr
		*/
		MULERT_API mule::Data::Basic::Type::DataHandler *GetDataHandler(const char16_t *name);

		/**
		 * @brief 按名称获取文件处理器
		 * @param name 处理器名称
		 * @return 取得的处理器，失败返回 nullptr
		*/
		MULERT_API mule::Data::Basic::Type::FileHandler *GetFileHandler(const char16_t *name);
	protected:
		std::list<void *> pluginHandlers;
		std::list<const PluginDescription *> descriptions;

	private:
		Mule();
	};
}
