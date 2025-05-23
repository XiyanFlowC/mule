#pragma once

#ifndef STRING_OBJECT_H__
#define STRING_OBJECT_H__

#include <Data/Basic/BasicType.h>
#include <Logger.h>

namespace mule
{
	namespace Data
	{
		namespace Basic
		{

			/**
			 * @brief String is not a typical object cause it holds a variable length.
			*/
			class String : public BasicType
			{
				Logger logger = Logger::GetLogger<String>();
			protected:
				size_t lastSize = -1;
			public:
				virtual size_t Size() const override;
				virtual std::u16string GetDataType() const override;

				virtual size_t GetLastSize() const override;
				virtual size_t EvalSize(const MultiValue &obj) const override;
			protected:
				virtual MultiValue DoRead(xybase::Stream *stream) override;
				virtual void DoWrite(xybase::Stream *stream, const MultiValue &value) override;
			};
		}
	}
}

#endif
