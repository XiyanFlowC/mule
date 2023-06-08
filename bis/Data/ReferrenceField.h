#ifndef REFERRENCE_FIELD_H__
#define REFERRENCE_FIELD_H__

#pragma once

#include "Basic/Field.h"
#include "Basic/Object.h"
#include "FieldCreator.h"
#include "Stringfier.h"
#include "ObjectManager.h"

namespace mule
{
	namespace Data
	{

		template <typename STRCLS = Stringfier, char PREFIX_SYMBOL = '@'>
		class ReferrenceField : public Basic::Field
		{
			Basic::Object *referent;

			ReferrenceField()
			{
				referent = nullptr;
			}
		public:
			class ReferrenceFieldCreator : public FieldCreator
			{
				// 通过 FieldCreator 继承
				virtual Basic::Field *DoCreateField(FieldCreatingInfo &info) override
				{
					if (!info.typeDescriptor.starts_with(PREFIX_SYMBOL))
					{
						return nullptr;
					}

					Basic::Object *innerType = ObjectManager::GetInstance().GetObject(info.typeDescriptor.substr(1));
					if (innerType == nullptr) return nullptr;

					ReferrenceField *referrer = new ReferrenceField();
					referrer->referent = innerType;
					return referrer;
				}
			};

			// 通过 Field 继承
			virtual Basic::MultiValue DoRead(FieldReadWriteContext &context) override
			{
				int ptr = context.stream->ReadInt32();
				size_t loc = context.stream->Tell();
				Basic::MultiValue mv;
				STRCLS handler(&mv);
					referent->Read(context.stream, &handler);
				context.stream->Seek(loc, 0);
				return mv;
			}

			virtual int DoWrite(FieldReadWriteContext &context, const Basic::MultiValue &value) const
			{
				return 0;
			}
		};
	}
}

#endif
