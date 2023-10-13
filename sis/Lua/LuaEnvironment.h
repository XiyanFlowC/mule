#pragma once

#ifndef LUA_API_H__
#define LUA_API_H__

#include "LuaHost.h"
#include <map>
#include <list>
#include <atomic>
#include <string>
#include <Stream.h>
#include <Data/Table.h>

namespace mule {
	namespace Lua {
		/**
		 * @brief Designed to provide the ability to operate the C++ environment for Lua scripts.
		 * This class will register lots of functions at the LuaHost.
		*/
		class LuaEnvironment
		{
			LuaEnvironment();

			std::map<int, xybase::Stream *> streams;
			std::atomic_int streamd;

		public:

			xybase::Stream *GetStream(int idx);

			int SetStream(xybase::Stream *stream);

			void CloseStream(int idx);

			/**
			 * @brief 获取Lua宿主环境
			 * @return 本环境
			*/
			static LuaEnvironment &GetInstance();

			/**
			 * @brief Register all functions to the vm / init the vm.
			*/
			static void Init();
		};
	}
}

#endif // end of LUA_API_H__
