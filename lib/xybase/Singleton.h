#ifndef XY_SINGLETON_H__
#define XY_SINGLETON_H__

namespace xybase
{
	template <typename T>
	class Singleton
	{
		T inst;
		Singleton<T>() {}
	public:
		static T& GetInstance()
		{
			static T inst;
			return inst;
		}
	};
}

#endif
