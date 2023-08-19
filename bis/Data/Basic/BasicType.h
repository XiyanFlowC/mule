#pragma once

#ifndef BASIC_TYPE_H__
#define BASIC_TYPE_H__

#include <string>

#include "Type.h"
#include "ContextManager.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			class BasicType : public Type
			{

			protected:

				std::u16string cacheVariableName;

				virtual MultiValue DoRead(xybase::Stream *stream) = 0;

				virtual void DoWrite(xybase::Stream *stream, const MultiValue &value) = 0;
				
			public:
				virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;

				virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) override;
			};
		}
	}
}

#endif
