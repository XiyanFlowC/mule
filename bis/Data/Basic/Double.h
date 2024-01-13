#pragma once

#ifndef DOUBLE_FIELD_H__
#define DOUBLE_FILED_H__

#include <Data/Basic/BasicType.h>

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			class Double : public BasicType
			{
			public:
				Double();

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
