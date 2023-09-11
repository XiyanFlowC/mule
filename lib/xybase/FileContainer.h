#pragma once

#ifndef XY_FILE_CONTAINER_H__
#define XY_FILE_CONTAINER_H__

#include <list>

#include "Stream.h"

namespace xybase
{
	class XY_API FileContainer
	{
	public:
		virtual ~FileContainer();

		virtual std::u16string GetName() = 0;

		enum FileOpenMode
		{
			FOM_READ,
			FOM_WRITE,
			FOM_READ_WRITE,
			FOM_TRUNCATE,
		};

		virtual void Flush() = 0;

		virtual xybase::Stream *Open(std::u16string name, FileOpenMode mode = FOM_READ_WRITE) = 0;

		virtual std::list<std::u16string> List() = 0;

		virtual void MakeDir(std::u16string path) = 0;

		virtual void Remove(std::u16string path, bool recursive = false) = 0;
	};
}

#endif
