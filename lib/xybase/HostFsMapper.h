#pragma once
#ifndef XY_HOST_FS_MAPPER_H__
#define XY_HOST_FS_MAPPER_H__

#include "FileContainer.h"
#include "xyapi.h"

namespace xybase
{
	/**
	 * @brief A mapper to the host file system. As a file container.
	*/
	class HostFsMapper : public FileContainer
	{
		std::u16string rootPath;

	public:
		XY_API HostFsMapper(std::u16string rootPath);

		XY_API ~HostFsMapper();

		XY_API std::u16string GetName() override;
		XY_API void Flush() override;
		XY_API xybase::Stream *Open(std::u16string name, FileOpenMode mode) override;
		XY_API std::list<std::u16string> List() override;
		XY_API void MakeDir(std::u16string path) override;
		XY_API void Remove(std::u16string path, bool recursive) override;
		XY_API bool Exists(std::u16string path) override;
	};
}


#endif // !XY_HOST_FS_MAPPER_H__
