#pragma once

#ifndef ARRAY_FIELD_H__
#define ARRAY_FIELD_H__

#include "Basic/Field.h"
#include "FieldCreator.h"
#include "Basic/Object.h"
#include "ObjectManager.h"
#include "../xybase/StringBuilder.h"
#include "../Exception/InvalidParameterException.h"

namespace mule
{
	namespace Data
	{

		class ArrayField : public Basic::Field
		{
		protected:
			Object *innerObject;
			int length;

		public:
			ArrayField(Object *innerObject, int length);

			class ArrayFieldCreator : public FieldCreator
			{
				// 通过 FieldCreator 继承
				virtual Basic::Field *DoCreateField(FieldCreatingInfo &info) override;
			};

			virtual size_t FieldSize() const override;

			// 通过 Field 继承
			virtual Basic::MultiValue DoRead(FieldReadWriteContext &context) override;
			virtual int DoWrite(FieldReadWriteContext &context, const Basic::MultiValue &value) const override;
		};
	}
}

#endif
