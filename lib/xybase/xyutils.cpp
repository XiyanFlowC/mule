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
    if (mode & PM_EXECUTE) mode_ |= 0111;
    if (mode & PM_READ) mode_ |= 0222;
    if (mode & PM_WRITE) mode_ |= 0444;
    return ::access(path, mode_);
#endif
}

int xybase::io::mkdir(const char *path)
{
#ifdef _WIN32
    return ::_mkdir(path);
#else
    return ::mkdir(path, 0777);
#endif
}
