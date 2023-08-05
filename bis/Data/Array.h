#pragma once

#ifndef ARRAY_H__
#define ARRAY_H__

#include "Basic/Type.h"
#include "TypeCreator.h"

namespace mule
{
	namespace Data
	{
		class Array : public Basic::Type
		{
		protected:
			std::string name;

			size_t length;

			Basic::Type *innerObject;
		public:
			class ArrayCreator : public TypeCreator
			{
			public:
				virtual Basic::Type *DoCreateObject(std::string info) override;
			};

			virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
			virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) override;
			virtual size_t Size() const override;
			virtual std::string GetTypeName() const override;
		};
	}
}

#endif
