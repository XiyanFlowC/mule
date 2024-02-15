#pragma once

#include "xyapi.h"
#include <list>
#include <functional>

namespace xybase
{
	template<typename ...Args>
	class Event
	{
		std::list<void(*)(Args...)> registeredCallbacks;

	public:
		void RegisterCallback(void(*callback)(Args...));

		void operator+= (void(*callback)(Args...));

		void UnregisterCallback(void(*callback)(Args...));

		void operator-= (void(*callback)(Args...));

		void Invoke(Args... args) const;

		void operator() (Args... args) const;
	};

	template<typename ...Args>
	inline void Event<Args...>::RegisterCallback(void(*callback)(Args...))
	{
		registeredCallbacks.push_back(callback);
	}

	template<typename ...Args>
	inline void Event<Args...>::operator+=(void(*callback)(Args...))
	{
		registeredCallbacks.push_back(callback);
	}

	template<typename ...Args>
	inline void Event<Args...>::UnregisterCallback(void(*callback)(Args...))
	{
		for (auto itr = registeredCallbacks.begin(); itr != registeredCallbacks.end(); ++itr)
		{
			if (*itr == callback)
			{
				registeredCallbacks.erase(itr);
			}
		}
	}

	template<typename ...Args>
	inline void Event<Args...>::operator-=(void(*callback)(Args...))
	{
		for (auto itr = registeredCallbacks.begin(); itr != registeredCallbacks.end(); ++itr)
		{
			if (*itr == callback)
			{
				registeredCallbacks.erase(itr);
			}
		}
	}

	template<typename ...Args>
	inline void Event<Args...>::Invoke(Args ...args) const
	{
		for (auto &&itr : registeredCallbacks)
		{
			itr(args...);
		}
	}

	template<typename ...Args>
	inline void Event<Args...>::operator()(Args ...args) const
	{
		for (auto &&itr : registeredCallbacks)
		{
			itr(args...);
		}
	}
}
