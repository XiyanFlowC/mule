#pragma once

#ifndef FLOAT_FIELD_H__
#define FLOAT_FIELD_H__

#include "Type.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			class Float : public Type
			{
			public:
				Float();

				virtual size_t Size() const override;

				virtual std::u16string GetTypeName() const override;


			protected:
				virtual MultiValue DoRead(xybase::Stream *stream) override;
				virtual void DoWrite(xybase::Stream *stream, const MultiValue &value) override;

			};
		}
	}
}

#endif