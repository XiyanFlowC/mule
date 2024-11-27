#pragma once

#ifndef DUMMY_FIELD_H__
#define DUMMY_FIELD_H__

#include <Exception/InvalidParameterException.h>
#include <Data/Basic/BasicType.h>
#include <Data/Basic/MultiValue.h>

namespace mule
{
	namespace Data
	{
		namespace Basic
		{

			class Dummy : public BasicType
			{
				int size;
			public:
				Dummy(int size);

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
