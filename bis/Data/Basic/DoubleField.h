#pragma once

#ifndef DOUBLE_FIELD_H__
#define DOUBLE_FILED_H__

#include "Field.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			class DoubleField : public Field
			{
				// ͨ�� Field �̳�
				virtual MultiValue DoRead(FieldReadWriteContext &context) override;
				virtual int DoWrite(FieldReadWriteContext &context, const MultiValue &value) const override;
			};
		}
	}
}

#endif
