#pragma once

#include <Data/Basic/Type.h>
#include <string>

namespace mule
{
	namespace Csv
	{
		class CsvFileHandler : public mule::Data::Basic::Type::FileHandler
		{
			mule::Data::Basic::MultiValue readElement;

			enum UCharMode {
				UCM_UTF8,
				UCM_UTF16LE,
				UCM_UTF16BE,
				UCM_UTF32LE, /* reserved, not implemented */
				UCM_UTF32BE, /* reserved, not implemented */
			} charMode = UCM_UTF8;
		protected:
			int ReadChar();

			std::u16string ReadCell();
		public:
			void OnRealmEnter(mule::Data::Basic::Type *realm, const std::u16string &name) override;
			void OnRealmExit(mule::Data::Basic::Type *realm, const std::u16string &name) override;
			void OnRealmEnter(mule::Data::Basic::Type *realm, int idx) override;
			void OnRealmExit(mule::Data::Basic::Type *realm, int idx) override;
			void OnSheetWriteStart() override;
			void OnSheetWriteEnd() override;
			const mule::Data::Basic::MultiValue OnDataWrite() override;
		};
	}
}
