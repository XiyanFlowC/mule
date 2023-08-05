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
			class ReferrenceObjectCreator : public TypeCreator
			{
				virtual Basic::Type *DoCreateObject(std::string info) override;
			};

			virtual void Read(xybase::Stream *stream, Basic::Type::DataHandler *dataHandler) override;
			virtual void Write(xybase::Stream *stream, Basic::Type::DataHandler *dataHandler) override;
			virtual size_t Size() const override;
			virtual std::string GetTypeName() const override;

		private:
			Referrence(Type *referent);
		};
	}
}

#endif
