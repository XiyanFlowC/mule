#pragma once

#ifndef LUA_API_H__
#define LUA_API_H__

#include "LuaHost.h"
#include <lua.hpp>

namespace mule {
	namespace Lua {
		/**
		 * @brief Designed to provide the ability to operate the C++ classes for Lua scripts.
		 * This class will register lots of functions at the LuaHost.
		*/
		class LuaApi
		{
		public:
			/**
			 * @brief Register all functions to the vm / init the vm.
			*/
			static void Init();
		};
	}
}

#endif // end of LUA_API_H__
