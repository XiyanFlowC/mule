#pragma once
#ifndef VAR_CHAR_H__
#define VAR_CHAR_H__

#include "Basic/Type.h"
#include "TypeCreator.h"
#include "../xybase/xyutils.h"

namespace mule
{
	namespace Data
	{
		class VarChar : Basic::Type
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

			virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
			virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) override;
			virtual size_t Size() const override;
			virtual std::u16string GetTypeName() const override;
		};
	}
}

#endif // !VAR_CHAR_H__
