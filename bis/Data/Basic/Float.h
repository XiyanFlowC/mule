#pragma once

#ifndef FLOAT_FIELD_H__
#define FLOAT_FIELD_H__

#include "Object.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			class Float : public Object
			{
			public:
				Float();

				virtual void Read(xybase::Stream *stream, DataHandler *handler) const override;
				virtual void Write(xybase::Stream *stream, DataHandler *handler) const override;
				virtual size_t Size() const override;

			protected:

				// ͨ�� Object �̳�
				virtual std::string GetTypeName() const override;
			};
		}
	}
}

#endif