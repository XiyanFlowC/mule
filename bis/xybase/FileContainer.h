#pragma once

#ifndef XY_FILE_CONTAINER_H__
#define XY_FILE_CONTAINER_H__

#include <list>

#include "Stream.h"

namespace xybase
{
	class FileContainer
	{
	public:
		enum FileOpenMode
		{
			FOM_READ,
			FOM_WRITE,
			FOM_READ_WRITE,
			FOM_TRUNCATE,
		};

		virtual xybase::Stream *Open(std::string name, FileOpenMode mode = FOM_READ_WRITE) = 0;

		virtual std::list<std::string> List() = 0;

		virtual std::string WorkDir() = 0;

		virtual void ChangeDir(std::string path) = 0;

		virtual void MakeDir(std::string path) = 0;

		virtual void Remove(std::string path, bool recursive = false) = 0;
	};
}

#endif
