#include <Mule.h>

#ifdef WIN32
#define ADVSTR_API __declspec(dll_export)
#else
#define ADVSTR_API
#endif

extern mule::PluginDescription desc;

extern "C"
{
	mule::PluginDescription *GetDescription()
	{
		return &desc;
	}
}
