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

			protected:
				// 通过 Type 继承
				virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
				virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) override;
				virtual size_t Size() const override;
				virtual std::string GetTypeName() const override;
			};
		}
	}
}

#endif
