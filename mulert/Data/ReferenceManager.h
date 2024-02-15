#pragma once

#ifndef MULE_REFERENCE_MANAGER_H__
#define MULE_REFERENCE_MANAGER_H__
#include "../mulert_api.h"
#include "Basic/MultiValue.h"
#include "Basic/Type.h"
#include <Stream.h>
#include <map>
#include <set>

namespace mule
{
	namespace Data
	{
		/**
		 * @brief 引用管理器，注册，甄别相同地址的引用。
		*/
		class MULERT_API ReferenceManager
		{
			std::map<xybase::Stream *, std::set<size_t>> records;

			static void StreamCloseHandler(xybase::Stream *sender);
		public:
			static ReferenceManager &GetInstance();

			/**
			 * @brief 尝试登录项，若该地址早已登录则返回假，否则返回真
			 * @param stream 项所属的流
			 * @param offset 确定的地址
			 * @return 若已经登录则为假，否则为真
			*/
			bool RegisterItem(xybase::Stream *stream, size_t offset);

			/**
			 * @brief 注销指定流，指定的流不存在时不做任何操作。
			 * @param stream 要注销的流
			*/
			void UnregisterItem(xybase::Stream *stream);
		};
	}
}

#endif
