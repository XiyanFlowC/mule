#pragma once

#ifndef DOUBLE_FIELD_H__
#define DOUBLE_FILED_H__

#include "Type.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			class Double : public Type
			{
			public:
				Double();

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
