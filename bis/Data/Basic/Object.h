#pragma once

#ifndef OBJECT_H__
#define OBJECT_H__

#include <map>
#include <string>
#include "MultiValue.h"
#include "../../xybase/Stream.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			class Object
			{
			public:

				class DataHandler
				{
				public:

					virtual void OnSheetReadStart();

					virtual void OnSheetReadEnd();

					virtual void OnSheetWriteStart();

					virtual void OnSheetWriteEnd();

					virtual void OnRealmEnter(Object *realm, std::string name) = 0;

					virtual void OnRealmExit(Object *realm, std::string name) = 0;

					virtual void OnRealmEnter(Object *realm, int idx) = 0;

					virtual void OnRealmExit(Object *realm, int idx) = 0;

					virtual void OnDataRead(const MultiValue &value) = 0;

					virtual MultiValue OnDataWrite() = 0;
				protected:
				};

				virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) const = 0;

				virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) const = 0;

				virtual size_t Size() const = 0;

				virtual std::string GetTypeName() const;
			};
		}
	}
}

#endif
