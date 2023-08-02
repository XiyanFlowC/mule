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
		private:
			LuaApi();

		public:
			/**
			 * @brief Get the instance of LuaApi.
			 * @return The instance of LuaApi.
			*/
			static LuaApi & GetInstance();

			/**
			 * @brief Register all functions to the vm / init the vm.
			*/
			void Init();
		};
	}
}

#endif // end of LUA_API_H__