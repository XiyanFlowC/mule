#pragma once

#ifndef INTEGER_FIELD_H__
#define INTEGER_FIELD_H__

#include "Field.h"
#include "../../Exception/InvalidParameterException.h"
#include "../../Exception/RestrictionViolationException.h"
#include "MultiValue.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{

			class IntegerField : public Field
			{
				int size;
				bool isUnsigned;
			public:
				IntegerField(int size, bool isUnsigned);

				virtual size_t Size() const override;

				virtual size_t FieldSize() const override;

			protected:
				virtual MultiValue DoRead(FieldReadWriteContext &context) override;
				virtual int DoWrite(FieldReadWriteContext &context, const MultiValue &value) const override;
			};
		}
	}
}

// TODO: 作为未来可能实现的功能之一
//class IntegerPartialField : public IntegerField
//{
//	bool isLastPart; // 指示该字段已是基本类型的最后一部分（若为真则清除 context，若为写则进行实际写入）
//	// 现在使用 size 作为此指引。如果需要特化逻辑时再使用本字段。
//	// int totalSize; // 指示该字段是否需要读取，读取多少位基本类型（若为 0 则不读取，而使用 context 中的数值）
//	int offset; // 指示该位域的偏移量
//protected:
//	virtual MultiValue DoRead(FieldReadWriteContext& context) override;
//	virtual int DoWrite(FieldReadWriteContext& context, const MultiValue &value) override;
//};
//
//class IntegerFieldCreator : public FieldCreator
//{
//	// 通过 FieldCreator 继承
//	virtual Field *DoCreateField(FieldCreatingInfo &info) override;
//};

#endif
