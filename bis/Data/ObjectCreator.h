#pragma once

#ifndef FIELD_CREATOR_H__
#define FIELD_CREATOR_H__

#include <string>

#include "Basic/Object.h"
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
		 * @brief 字段创建器，ObjectManager用
		*/
		class ObjectCreator
		{
		public:
			ObjectCreator();

			ObjectCreator(ObjectCreator *next);

			ObjectCreator(ObjectCreator &pattern);

			ObjectCreator *nextCreator;

			/**
			 * @brief Try to create a field handler by given information.
			 * @param info
			 * @return
			*/
			Basic::Object *GetOrCreateObject(std::string info);

		protected:
			// Return nullptr so that flow can move to the next ring of chain-of-responsiblity
			virtual Basic::Object *DoCreateObject(std::string info) = 0;
		};

		/**
		 * @brief 基本字段创建器，用于创建整形、浮点型等基础读写器。
		*/
		class BasicFieldCreator : public ObjectCreator
		{
			// 通过 FieldCreator 继承
			virtual Basic::Object *DoCreateObject(std::string info) override;
		};
	}
}

#endif
