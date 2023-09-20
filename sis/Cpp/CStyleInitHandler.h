#pragma once

#ifndef CSTYLE_INIT_HANDLER_H__
#define CSTYLE_INIT_HANDLER_H__

#include <Data/Basic/Type.h>
#include <StringBuilder.h>
#include <Exception/InvalidOperationException.h>

namespace mule
{
	namespace Cpp
	{
		class CStyleInitHandler : public mule::Data::Basic::Type::DataHandler
		{
		public:
			CStyleInitHandler();

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
			xybase::Stream *stream;

			enum {
				CSIHS_IDLE,
				CSIHS_READ,
				CSIHS_WRITE,
			} state;

			int layer = 0;
		};
	}
}

#endif // !CSTYLE_INIT_HANDLER_H__

