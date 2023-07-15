#pragma once

#ifndef DOUBLE_FIELD_H__
#define DOUBLE_FILED_H__

#include "Object.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			class Double : public Object
			{
			public:
				Double();

			protected:
				// ͨ�� Object �̳�
				virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) const override;
				virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) const override;
				virtual size_t Size() const override;
				virtual std::string GetTypeName() const override;
			};
		}
	}
}

#endif
