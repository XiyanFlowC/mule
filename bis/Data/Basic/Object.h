#pragma once

#ifndef OBJECT_H__
#define OBJECT_H__

#include <map>
#include <string>
#include "../../xybase/Stream.h"
#include "DataHandler.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			class Object
			{
			public:
				virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) = 0;

				virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) const = 0;

				virtual size_t Size() const = 0;
			};
		}
	}
}

#endif
