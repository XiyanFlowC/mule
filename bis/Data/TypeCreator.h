#pragma once

#ifndef FIELD_CREATOR_H__
#define FIELD_CREATOR_H__

#include <string>

#include "Basic/Type.h"
#include "Basic/Integer.h"
#include "Basic/Double.h"
#include "Basic/Float.h"
#include "Basic/String.h"
#include "Basic/MultiValue.h"

namespace mule
{
	namespace Data
	{

		/**
		 * @brief �ֶδ�������ObjectManager��
		*/
		class TypeCreator
		{
		public:
			TypeCreator();

			TypeCreator(TypeCreator *next);

			TypeCreator(TypeCreator &pattern);

			TypeCreator *nextCreator;

			/**
			 * @brief Try to create a field handler by given information.
			 * @param info
			 * @return
			*/
			Basic::Type *GetOrCreateObject(std::string info);

		protected:
			// Return nullptr so that flow can move to the next ring of chain-of-responsiblity
			virtual Basic::Type *DoCreateObject(std::string info) = 0;
		};

		/**
		 * @brief �����ֶδ����������ڴ������Ρ������͵Ȼ�����д����
		*/
		class BasicFieldCreator : public TypeCreator
		{
			// ͨ�� FieldCreator �̳�
			virtual Basic::Type *DoCreateObject(std::string info) override;
		};
	}
}

#endif
