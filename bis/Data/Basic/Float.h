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

				virtual void Read(xybase::Stream *stream, DataHandler *handler) override;
				virtual void Write(xybase::Stream *stream, DataHandler *handler) override;
				virtual size_t Size() const override;

			protected:

				// 通过 Type 继承
				virtual std::string GetTypeName() const override;
			};
		}
	}
}

#endif