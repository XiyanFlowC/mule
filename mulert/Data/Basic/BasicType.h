#pragma once

#ifndef BASIC_TYPE_H__
#define BASIC_TYPE_H__

#include <string>

#include "../../mulert_api.h"
#include "Type.h"
#include "ContextManager.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			class BasicType : public Type
			{

			protected:
				MULERT_API void DoConstraintCheck(const MultiValue &value);
				
			public:

				MULERT_API virtual MultiValue DoRead(xybase::Stream *stream) = 0;

				MULERT_API virtual void DoWrite(xybase::Stream *stream, const MultiValue &value) = 0;


				MULERT_API virtual ~BasicType();

				enum ConstraintType
				{
					BTCT_NONE,
					BTCT_EQ,
					BTCT_LT,
					BTCT_GT,
					BTCT_NEQ,
					BTCT_NLT,
					BTCT_NGT
				} constraintType = BTCT_NONE;

				MultiValue comparator = MultiValue::MV_NULL;

				class ConstraintViolationException : public xybase::RuntimeException
				{
					public:
						MULERT_API ConstraintViolationException(const std::wstring &msg);

						MULERT_API ~ConstraintViolationException();
				};

				std::u16string cacheVariableName;

				MULERT_API virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;

				MULERT_API virtual void Write(xybase::Stream *stream, FileHandler *fileHandler) override;
			};
		}
	}
}

#endif
