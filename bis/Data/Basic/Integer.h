#pragma once

#ifndef INTEGER_FIELD_H__
#define INTEGER_FIELD_H__

#include <Exception/InvalidParameterException.h>
#include "Type.h"
#include "MultiValue.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{

			class Integer : public Type
			{
				int size;
				bool isUnsigned;
			public:
				Integer(int size, bool isUnsigned);

				virtual size_t Size() const override;

				virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
				virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) override;
				virtual std::u16string GetTypeName() const override;

			protected:
			};
		}
	}
}

#endif
