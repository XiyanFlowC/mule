#pragma once

#ifndef XYUTILS_H__
#define XYUTILS_H__

#define XY_ALIGN(n, b) ((n) < 0 ? (n) & ~((b) - 1) : ((n + (b) - 1) & ~((b) - 1))

#include "xystring.h"

namespace xybase
{
	namespace io
	{
		enum AccessMode
		{
			PM_READ = 0x1,
			PM_WRITE = 0x2,
			PM_READWRITE = 0X3,
			PM_EXECUTE = 0x4,
		};

		int access(const char *path, AccessMode mode);

		int mkdir(const char *path);
	}
}

#endif // !XYUTILS_H__
