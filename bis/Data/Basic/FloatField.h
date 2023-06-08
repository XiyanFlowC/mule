#pragma once

#ifndef FLOAT_FIELD_H__
#define FLOAT_FIELD_H__

#include "Field.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			class FloatField : public Field
			{
				// ͨ�� Field �̳�
				virtual MultiValue DoRead(FieldReadWriteContext &context) override;
				virtual int DoWrite(FieldReadWriteContext &context, const MultiValue &value) const override;
			};
		}
	}
}

#endif