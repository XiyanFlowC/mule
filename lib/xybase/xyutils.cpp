#include "xyutils.h"
#include <cstdint>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

int xybase::io::access(const char *path, AccessMode mode)
{
#ifdef _WIN32
    int mode_ = 0;
    if (mode & PM_READ) mode_ |= 0x2;
    if (mode & PM_WRITE) mode_ |= 0x4;
    return ::_access(path, mode_);
#else
    int mode_ = 0;
    if (mode & PM_EXECUTE) mode_ |= X_OK;
    if (mode & PM_READ) mode_ |= R_OK;
    if (mode & PM_WRITE) mode_ |= W_OK;
    return ::access(path, mode_);
#endif
}

int xybase::io::mkdir(const char *path)
{
#ifdef _WIN32
    return ::_mkdir(path);
#else
    return ::mkdir(path, 0755);
#endif
}

int xybase::io::CreateDirectoryRecursively(const std::string &path, size_t index)
{
	size_t endIndex = path.find_first_of('/', index);

	if (endIndex == std::string::npos)
	{
		if (xybase::io::access(path.c_str(), xybase::io::PM_READ | xybase::io::PM_WRITE))
		{
			return xybase::io::mkdir(path.c_str());
		}
	}

	std::string curPath = path.substr(0, endIndex);
	if (xybase::io::access(curPath.c_str(), xybase::io::PM_READ | xybase::io::PM_WRITE))
	{
		xybase::io::mkdir(curPath.c_str());
	}
	return CreateDirectoryRecursively(path, endIndex + 1);
}

int xybase::io::CreateDirectoryRecursively(const char *path)
{
	return CreateDirectoryRecursively(path, 0);
}
