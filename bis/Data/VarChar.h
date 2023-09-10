#pragma once
#ifndef VAR_CHAR_H__
#define VAR_CHAR_H__

#include "Basic/BasicType.h"
#include "TypeCreator.h"
#include "../xybase/xyutils.h"

namespace mule
{
	namespace Data
	{
		class VarChar : Basic::BasicType
		{
		protected:
			size_t length;

			VarChar(size_t length);

			char *buffer;
		public:
			~VarChar();

			class VarCharCreator : public TypeCreator
			{
			public:

				virtual Basic::Type *DoCreateObject(std::u16string info) override;
			};

			virtual size_t Size() const override;
			virtual std::u16string GetDataType() const override;

			virtual mule::Data::Basic::MultiValue DoRead(xybase::Stream *stream) override;
			virtual void DoWrite(xybase::Stream *stream, const mule::Data::Basic::MultiValue &value) override;
		};
	}
}

#endif // !VAR_CHAR_H__
