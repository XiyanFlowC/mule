#pragma once

#include <Data/Basic/Type.h>
#include <string>
#include <Logger.h>

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

			/**
			 * @brief Flag: End of line read.
			 */
			bool flg_eol;
			// 换行检查
			int wrapLayer = 1;
			/**
			 * @brief 抑制换行：1-抑制基本类型，2-抑制组合类型，0-不抑制
			*/
			int wrapSuppression = 0;
			int layer = 0;

			Logger logger = Logger::GetLogger<CsvFileHandler>();
		protected:
			int ReadChar();

			std::u16string ReadCell();
		public:
			void OnRealmEnter(mule::Data::Basic::Type *realm, const std::u16string &name) override;
			void OnRealmExit(mule::Data::Basic::Type *realm, const std::u16string &name) override;
			void HandleReamExit(mule::Data::Basic::Type *realm);
			void OnRealmEnter(mule::Data::Basic::Type *realm, int idx) override;
			void HandleRealmEnter(mule::Data::Basic::Type *realm);
			void OnRealmExit(mule::Data::Basic::Type *realm, int idx) override;
			void OnSheetWriteStart() override;
			void OnSheetWriteEnd() override;
			const mule::Data::Basic::MultiValue OnDataWrite() override;
		};
	}
}
