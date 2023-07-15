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
				// 通过 FieldCreator 继承
				virtual Basic::Object *DoCreateObject(std::string info) override
				{
					if (!info.starts_with("ref:"))
					{
						return nullptr;
					}

					Basic::Object *innerType = ObjectManager::GetInstance().GetObject(info.substr(4));
					if (innerType == nullptr) return nullptr;

					Referrence *referrer = new Referrence(innerType);
					return referrer;
				}
			};

			// 通过 Object 继承
			virtual void Read(xybase::Stream *stream, Basic::Object::DataHandler *dataHandler) const override;
			virtual void Write(xybase::Stream *stream, Basic::Object::DataHandler *dataHandler) const override;
			virtual size_t Size() const override;
			virtual std::string GetTypeName() const override;

		private:
			Referrence(Object *referent);
		};
	}
}

#endif
