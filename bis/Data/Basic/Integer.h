#pragma once

#ifndef INTEGER_FIELD_H__
#define INTEGER_FIELD_H__

#include <Exception/InvalidParameterException.h>
#include "BasicType.h"
#include "MultiValue.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{

			class Integer : public BasicType
			{
				int size;
				bool isUnsigned;
			public:
				Integer(int size, bool isUnsigned);

				virtual size_t Size() const override;

				virtual std::u16string GetDataType() const override;
			protected:
				virtual MultiValue DoRead(xybase::Stream *stream) override;

				virtual void DoWrite(xybase::Stream *stream, const MultiValue &value) override;

			};
		}
	}
}

#endif
