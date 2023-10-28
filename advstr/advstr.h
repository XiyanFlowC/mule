#include <Mule.h>

#ifdef WIN32
#define ADVSTR_API __declspec(dllexport)
#else
#define ADVSTR_API
#endif

extern mule::PluginDescription desc;

extern "C"
{
	mule::PluginDescription ADVSTR_API *GetDescription()
	{
		return &desc;
	}
}
