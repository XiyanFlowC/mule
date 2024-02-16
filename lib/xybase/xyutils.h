#pragma once

#ifndef XYUTILS_H__
#define XYUTILS_H__

#define XY_ALIGN(n, b) ((n) < 0 ? (n) & ~((b) - 1) : ((n + (b) - 1) & ~((b) - 1)))
#define XY_MAX(a, b) ((a) > (b) ? (a) : (b))
#define XY_MIN(a, b) ((a) > (b) ? (b) : (a))

#include "xyapi.h"
#include <string>

namespace xybase
{
	namespace io
	{
		using AccessMode = int;

		const AccessMode PM_READ = 0x1;
		const AccessMode PM_WRITE = 0x2;
		const AccessMode PM_EXECUTE = 0x4;

		/**
		 * @brief 获得是否能以指定权限访问指定路径
		 * @param path 指定路径
		 * @param mode 要测试的访问权限
		 * @return 
		*/
		int XY_API access(const char *path, AccessMode mode);

		int XY_API mkdir(const char *path);

		int XY_API CreateDirectoryRecursively(const std::string &path, size_t index = 0);

		int XY_API CreateDirectoryRecursively(const char *path);
	}
}

#endif // !XYUTILS_H__
