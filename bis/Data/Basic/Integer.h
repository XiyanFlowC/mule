#pragma once

#ifndef INTEGER_FIELD_H__
#define INTEGER_FIELD_H__

#include "Object.h"
#include "../../Exception/InvalidParameterException.h"
#include "../../Exception/RestrictionViolationException.h"
#include "MultiValue.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{

			class Integer : public Object
			{
				int size;
				bool isUnsigned;
			public:
				Integer(int size, bool isUnsigned);

				virtual size_t Size() const override;

				virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) const override;
				virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) const override;
				virtual std::string GetTypeName() const override;

			protected:
			};
		}
	}
}

#endif
