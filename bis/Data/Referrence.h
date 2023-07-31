#ifndef REFERRENCE_FIELD_H__
#define REFERRENCE_FIELD_H__

#pragma once

#include "Basic/Object.h"
#include "ObjectCreator.h"
#include "ObjectManager.h"

namespace mule
{
	namespace Data
	{
		class Referrence : public Basic::Object
		{
			Basic::Object *referent = nullptr;
		public:
			class ReferrenceObjectCreator : public ObjectCreator
			{
				virtual Basic::Object *DoCreateObject(std::string info) override;
			};

			virtual void Read(xybase::Stream *stream, Basic::Object::DataHandler *dataHandler) override;
			virtual void Write(xybase::Stream *stream, Basic::Object::DataHandler *dataHandler) override;
			virtual size_t Size() const override;
			virtual std::string GetTypeName() const override;

		private:
			Referrence(Object *referent);
		};
	}
}

#endif
