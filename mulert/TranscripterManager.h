#pragma once

#include "mulert_api.h"
#include <list>
#include <Stream.h>

namespace mule
{
	/**
	 * @brief 管理Stream转写类的管理器。该类转写类模拟为一个Stream，对下层Stream行为进行转换。
	*/
	class TranscripterManager
	{
		std::list<xybase::Stream *(*)(const char16_t *name, xybase::Stream *infraStream)> streamCreators;

		std::list<xybase::Stream *(*)(int count, const char16_t **parameter)> advancedStreamCreators;

	public:

		MULERT_API static TranscripterManager &GetInstance();

		/**
		 * @brief 将一个现有的流以指定的转译器转译
		 * @param name 转译器标识符
		 * @param infraStream 要被转译的流
		 * @return 转译后的流（转译器对象）
		*/
		MULERT_API xybase::Stream *Transcript(const char16_t *name, xybase::Stream *infraStream);

		/**
		 * @brief 以指定的参数打开一个流
		 * @param count 参数个数
		 * @param parameter 参数
		 * @return 打开的流
		*/
		MULERT_API xybase::Stream *Transcript(int count, const char16_t **parameter);

		/**
		 * @brief 注册转译器创建函数
		 * @param creator 创建函数
		*/
		MULERT_API void RegisterCreator(xybase::Stream *(*creator)(const char16_t *name, xybase::Stream *infraStream));

		/**
		 * @brief 注册流创建函数
		 * @param creator 创建函数
		*/
		MULERT_API void RegisterCreator(xybase::Stream *(*creator)(int count, const char16_t **parameter));
	};
}
