#pragma once

#include "../mulert_api.h"
#include "Basic/BasicType.h"
#include "TypeCreator.h"
#include "../Logger.h"
#include <Fragment/FragmentManager.h>

namespace mule
{
	namespace Data
	{
		class SmartReference : public Basic::BasicType
		{
		protected:
			/**
			 * @brief 内部的类型。
			*/
			Basic::BasicType *referent = nullptr;

			int GetAlign() const;
		public:
			class MemoryManager
			{
				mule::Logger logger = mule::Logger::GetLogger<MemoryManager>();

				std::map<std::u16string, xybase::Fragment::FragmentManager> memories;

				std::map<std::u16string, std::map<mule::Data::Basic::BasicType*, std::map<Basic::MultiValue, size_t>>> assignedAddresses;

				MemoryManager();
			public:

				MULERT_API void DisposeStreamRecords(xybase::Stream *stream);

				MULERT_API static MemoryManager &GetInstance();

				MULERT_API xybase::Fragment::FragmentManager &GetMemory(xybase::Stream *stream);

				MULERT_API size_t AssignFor(xybase::Stream *stream, const mule::Data::Basic::MultiValue &value, mule::Data::Basic::BasicType *type, size_t size, int align);

				MULERT_API bool IsAssigned(xybase::Stream *stream, const mule::Data::Basic::MultiValue &value, mule::Data::Basic::BasicType *type);

				MULERT_API void SaveFreeSpace();
			};

			MULERT_API SmartReference(Basic::BasicType *referent);

			MULERT_API size_t Size() const override;

			MULERT_API std::u16string GetDataType() const override;

			MULERT_API Basic::MultiValue DoRead(xybase::Stream *stream) override;

			MULERT_API void DoWrite(xybase::Stream *stream, const Basic::MultiValue &value) override;

			MULERT_API bool IsComposite() const override;

			class MULERT_API SmartReferenceCreator : public TypeCreator
			{
			public:

				mule::Data::Basic::Type *DoCreateObject(const std::u16string &info) override;
				mule::Data::Basic::Type *DoCreateObject(const std::u16string &info, const std::map<std::u16string, std::u16string> &metainfo) override;
			};

		};
	}
}
