#pragma once

#ifndef LUA_HOST_H__
#define LUA_HOST_H__

#include <lua.hpp>
#include <varargs.h>

#include <Data/Basic/MultiValue.h>
#include <Exception/Exception.h>

namespace mule
{
	namespace Lua
	{
		/**
		 * @brief Lua 执行时发生的错误
		*/
		class LuaException : public mule::Exception::Exception
		{
		public:
			LuaException(std::string desc, const char *file, int line);
		};

		/**
		 * @brief Lua 脚本在本机程序的宿主。
		*/
		class LuaHost
		{
			lua_State *L;

			LuaHost();

			~LuaHost();

		public:
			/**
			 * @brief Get instance of lua host.
			 * @return A instance of lua host.
			*/
			static LuaHost &GetInstance();

			/**
			 * @brief Load lua standared libraries to the current Lua VM.
			*/
			void LoadLuaStandardLibs();

			/**
			 * @brief Load and run a script.
			 * @param path The path to the script file.
			*/
			void RunScript(const char *path);

			/**
			 * @brief Load a script.
			 * @param path The path to the script file.
			*/
			void LoadScript(const char *path);

			/**
			 * @brief Register a function to lua vm.
			 * @param name Name of the function, suggest to using cpp_ as prefix.
			 * @param func The function need to be registered.
			*/
			void RegisterFunction(const std::string& name, lua_CFunction func);

			/**
			 * @brief Calling luaL_newLib()
			 * @param libRegTable The register table.
			*/
			void RegisterLibrary(luaL_Reg *libRegTable);

			/**
			 * @brief Get a global variable from lua vm.
			 * @param name The name of the global variable.
			 * @return The value of the specified variable.
			*/
			mule::Data::Basic::MultiValue GetGlobal(const std::string& name);

			/**
			 * @brief Set a global variable to lua vm.
			 * @param name The name of the global variable.
			 * @param value The value of the global variable.
			*/
			void SetGlobal(const std::string &name, const mule::Data::Basic::MultiValue& value);

			/**
			 * @brief Call an Lua function.
			 * @param name The name of the function.
			 * @param count The count of the var. args.
			 * @param ... args, should be MultiValue.
			*/
			mule::Data::Basic::MultiValue Call(const std::string name, int count, ...);

			/**
			 * @brief Reset the lua stack.
			*/
			void ResetStack();

			/**
			 * @brief Get the top index of lua stack
			 * @return The top index
			*/
			int GetStackTop();

			/**
			 * @brief Set the top index of lua stack
			*/
			void SetStackTop(int idx);

		private:
			mule::Data::Basic::MultiValue PopValue();

			void PushValue(const mule::Data::Basic::MultiValue &v);
		};
	}
}

#endif
