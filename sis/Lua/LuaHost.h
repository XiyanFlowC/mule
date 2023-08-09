#pragma once

#ifndef LUA_HOST_H__
#define LUA_HOST_H__

#include <lua.hpp>
#include <functional>
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
			bool disposable = false;

			LuaHost();

			~LuaHost();

		public:
			/**
			 * @brief Get instance of lua host.
			 * @return A instance of lua host.
			*/
			static LuaHost &GetInstance();

			LuaHost(lua_State *L);

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

			template<typename RetT, typename... Args>
			void RegisterFunction(const std::string &name, RetT(*func)(Args...));

			/**
			 * @brief Get a global variable from lua vm.
			 * @param name The name of the global variable.
			 * @return The value of the specified variable.
			*/
			mule::Data::Basic::MultiValue GetGlobal(const std::string& name);

			/**
			 * @brief Get a value from given index on the stack.
			 * @param idx Index of the value.
			 * @return The value required.
			*/
			mule::Data::Basic::MultiValue GetValue(int idx);

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

			template<typename AT>
			bool CheckArg(int index);

			template<typename AT, typename ...ArgTs>
			bool CheckArgs(int index);

			template<typename T>
			T GetArg(int index);

			template<typename T, typename... Args, size_t... Indices>
			T CallCFunc(T(*func)(Args...), std::index_sequence<Indices...>);
		};

		template<typename T>
		T LuaHost::GetArg(int index)
		{
			index += 1;
			if constexpr (std::is_same<T, std::string>::value)
			{
				return lua_tostring(L, index);
			}
			else if constexpr (std::is_integral_v<T>)
			{
				return lua_tointeger(L, index);
			}
			else if constexpr (std::is_floating_point_v<T>)
			{
				return lua_tonumber(L, index);
			}
			else return 0;
		}

		template<typename T, typename ...Args, size_t ...Indices>
		T LuaHost::CallCFunc(T(*func)(Args...), std::index_sequence<Indices...>)
		{
			return func(GetArg<Args>(Indices)...);
		}

		template<typename AT>
		bool LuaHost::CheckArg(int index)
		{
			if constexpr (std::is_same_v<AT, std::string>)
			{
				return lua_isstring(L, index);
			}
			else if constexpr (std::is_integral_v<AT>)
			{
				return lua_isinteger(L, index);
			}
			else if constexpr (std::is_floating_point_v<AT>)
			{
				return lua_isnumber(L, index);
			}
			return false;
		}

		template<typename AT, typename ...ArgTs>
		bool LuaHost::CheckArgs(int index)
		{
			if constexpr (sizeof...(ArgTs) == 0)
				return CheckArg<AT>(index);
			else
				return CheckArg<AT>(index) && CheckArgs<ArgTs>(index + 1);
		}

		template<typename RetT, typename ...Args>
		void LuaHost::RegisterFunction(const std::string &name, RetT (*func)(Args...))
		{
			auto wrapper = [](lua_State *Ls)->int {
				LuaHost host{ Ls };

				if (!lua_islightuserdata(Ls, lua_upvalueindex(1)))
				{
					host.PushValue(-1);
					return 1;
				}

				if (!host.CheckArgs<Args...>(1))
				{
					host.PushValue(-1ll);
					return 1;
				}

				RetT(*f)(Args...) = reinterpret_cast<RetT(*)(Args...)>(lua_touserdata(Ls, lua_upvalueindex(1)));

				RetT ret = host.CallCFunc(f, std::index_sequence_for<Args...>{});

				lua_pop(Ls, sizeof...(Args));
				
				host.PushValue(mule::Data::Basic::MultiValue(ret));
				return 1;
			};
			lua_pushlightuserdata(L, func);
			lua_pushcclosure(L, wrapper, 1);
			lua_setglobal(L, name.c_str());
		}
	}
}

#endif
