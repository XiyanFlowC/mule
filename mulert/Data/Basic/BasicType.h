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
			class MULERT_API BasicType : public Type
			{

			protected:

				virtual MultiValue DoRead(xybase::Stream *stream) = 0;

				virtual void DoWrite(xybase::Stream *stream, const MultiValue &value) = 0;

				void DoConstraintCheck(const MultiValue &value);
				
			public:

				virtual ~BasicType();

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
						ConstraintViolationException(const std::wstring &msg);
				};

				std::u16string cacheVariableName;

				virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;

				virtual void Write(xybase::Stream *stream, FileHandler *fileHandler) override;
			};
		}
	}
}

#endif
