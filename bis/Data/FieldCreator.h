#pragma once

#ifndef FIELD_CREATOR_H__
#define FIELD_CREATOR_H__

#include <string>

#include "Basic/Field.h"
#include "Basic/IntegerField.h"
#include "Basic/DoubleField.h"
#include "Basic/FloatField.h"
#include "Basic/MultiValue.h"

namespace mule
{
	namespace Data
	{

		/**
		 * @brief 字段创建器，ObjectManager用
		*/
		class FieldCreator
		{
		public:
			FieldCreator();

			FieldCreator(FieldCreator *next);

			FieldCreator(FieldCreator &pattern);

			FieldCreator *nextCreator;

			struct FieldCreatingInfo
			{
				// General description
				std::string typeDescriptor;

				// ------ Field Definitions/Restrictions ------
				int width; // width occupied by the field.
				int occupiedWidth; // width already occupied by the field.
				//bool completeField; // the basic write element was ready to write/read.

				bool isConstant; // to specify if this field is a constant.
				Basic::MultiValue constant; // the constant this field should be.
			};

			/**
			 * @brief Try to create a field handler by given information.
			 * @param info
			 * @return
			*/
			Basic::Field *CreateField(FieldCreatingInfo &info);

		protected:
			// Return nullptr so that flow can move to the next ring of chain-of-responsiblity
			virtual Basic::Field *DoCreateField(FieldCreatingInfo &info) = 0;
		};

		/**
		 * @brief 基本字段创建器，用于创建整形、浮点型等基础读写器。
		*/
		class BasicFieldCreator : public FieldCreator
		{
			// 通过 FieldCreator 继承
			virtual Basic::Field *DoCreateField(FieldCreator::FieldCreatingInfo &info) override;
		};
	}
}

#endif
