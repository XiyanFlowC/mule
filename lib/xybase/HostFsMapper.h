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
	class XY_API HostFsMapper : public FileContainer
	{
		std::u16string rootPath;

	public:
		HostFsMapper(std::u16string rootPath);

		std::u16string GetName() override;
		void Flush() override;
		xybase::Stream *Open(std::u16string name, FileOpenMode mode) override;
		std::list<std::u16string> List() override;
		void MakeDir(std::u16string path) override;
		void Remove(std::u16string path, bool recursive) override;
	};
}


#endif // !XY_HOST_FS_MAPPER_H__
