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

				std::map<std::u16string, std::map<Basic::MultiValue, size_t>> assign;

			public:
				MemoryManager();

				void DisposeStream(xybase::Stream *stream);

				static MemoryManager &GetInstance();

				xybase::Fragment::FragmentManager &GetMemory(xybase::Stream *stream);

				size_t AssignFor(xybase::Stream *stream, const mule::Data::Basic::MultiValue &value, size_t size, int align);

				void SaveFreeSpace();
			};

			SmartReference(Basic::BasicType *referent);

			size_t Size() const override;

			std::u16string GetDataType() const override;

			Basic::MultiValue DoRead(xybase::Stream *stream) override;

			void DoWrite(xybase::Stream *stream, const Basic::MultiValue &value) override;

			bool IsComposite() const override;

			class SmartReferenceCreator : public TypeCreator
			{
			public:

				mule::Data::Basic::Type *DoCreateObject(const std::u16string &info) override;
				mule::Data::Basic::Type *DoCreateObject(const std::u16string &info, const std::map<std::u16string, std::u16string> &metainfo) override;
			};

		};
	}
}
