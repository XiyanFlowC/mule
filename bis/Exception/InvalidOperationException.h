#pragma once

#ifndef INVALID_OPERATION_H__
#define INVALID_OPERATION_H__

#include "Exception.h"
#include <string>

namespace mule
{
	namespace Exception
	{

		/**
		 * @brief 无效操作。
		 * 表示对象因当前状态或设计原因而无法处理指定操作的异常。
		*/
		class InvalidOperationException : public Exception
		{
		public:
			/**
			 * @brief 初始化无效操作异常。
			 * @param description 对异常信息的描述。
			 * @param file 抛出异常的文件。
			 * @param line 抛出异常的行号。
			*/
			InvalidOperationException(std::string description, const char *file, int line);
		};
	}
}

#endif
