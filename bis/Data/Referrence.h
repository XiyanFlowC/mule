#ifndef REFERRENCE_FIELD_H__
#define REFERRENCE_FIELD_H__

#pragma once

#include "Basic/Type.h"
#include "TypeCreator.h"
#include "TypeManager.h"

namespace mule
{
	namespace Data
	{
		class Referrence : public Basic::Type
		{
			Basic::Type *referent = nullptr;
		public:
			class ReferrenceCreator : public TypeCreator
			{
				virtual Basic::Type *DoCreateObject(std::u16string info) override;
			};

			virtual void Read(xybase::Stream *stream, Basic::Type::DataHandler *dataHandler) override;
			virtual void Write(xybase::Stream *stream, Basic::Type::DataHandler *dataHandler) override;
			virtual size_t Size() const override;
			virtual std::u16string GetTypeName() const override;

		private:
			Referrence(Type *referent);

			virtual mule::Data::Basic::MultiValue DoRead(xybase::Stream *stream) override;
			virtual void DoWrite(xybase::Stream *stream, const mule::Data::Basic::MultiValue &value) override;
		};
	}
}

#endif
