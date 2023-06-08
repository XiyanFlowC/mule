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

// TODO: ��Ϊδ������ʵ�ֵĹ���֮һ
//class IntegerPartialField : public IntegerField
//{
//	bool isLastPart; // ָʾ���ֶ����ǻ������͵����һ���֣���Ϊ������� context����Ϊд�����ʵ��д�룩
//	// ����ʹ�� size ��Ϊ��ָ���������Ҫ�ػ��߼�ʱ��ʹ�ñ��ֶΡ�
//	// int totalSize; // ָʾ���ֶ��Ƿ���Ҫ��ȡ����ȡ����λ�������ͣ���Ϊ 0 �򲻶�ȡ����ʹ�� context �е���ֵ��
//	int offset; // ָʾ��λ���ƫ����
//protected:
//	virtual MultiValue DoRead(FieldReadWriteContext& context) override;
//	virtual int DoWrite(FieldReadWriteContext& context, const MultiValue &value) override;
//};
//
//class IntegerFieldCreator : public FieldCreator
//{
//	// ͨ�� FieldCreator �̳�
//	virtual Field *DoCreateField(FieldCreatingInfo &info) override;
//};

#endif
