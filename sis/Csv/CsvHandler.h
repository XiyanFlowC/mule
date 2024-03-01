#pragma once

#ifndef CSV_HANDLER_H__
#define CSV_HANDLER_H__

#include <Data/Basic/Type.h>

namespace mule
{
	namespace Csv
	{
		class CsvOutHandler : public mule::Data::Basic::Type::DataHandler
		{
		public:
			/**
			 * @brief 缩进符号个数
			*/
			static int ident;
			/**
			 * @brief 缩进风格，0 为 tab，1 为空格
			*/
			static int type;

			virtual void OnSheetReadStart() override;

			virtual void OnSheetReadEnd() override;

			virtual void OnRealmEnter(mule::Data::Basic::Type *realm, const std::u16string &name) override;
			virtual void OnRealmExit(mule::Data::Basic::Type *realm, const std::u16string &name) override;
			virtual void OnRealmEnter(mule::Data::Basic::Type *realm, int idx) override;
			virtual void OnRealmExit(mule::Data::Basic::Type *realm, int idx) override;
			virtual void OnDataRead(const mule::Data::Basic::MultiValue &value) override;
		private:
			enum {
				CHS_IDLE,
				CHS_READ,
				CHS_READ_TRAILCELL,
				CHS_WRITE,
				CHS_WRITE_TRAILCELL,
			} status = CHS_IDLE;

			bool disarmed;
			bool isString, isText;
			int layer = 0;
			int wrapLayer = 1;
			/**
			 * @brief 抑制换行：1-抑制基本类型，2-抑制组合类型，0-不抑制
			*/
			int wrapSuppression = 0;
		};
	}
}

#endif /* End of Csv_HANDLER_H__*/
