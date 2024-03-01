#pragma once

#include <Data/Basic/Type.h>

namespace mule
{
	namespace Json
	{
		class JsonOutHandler : public mule::Data::Basic::Type::DataHandler
		{
			std::u16string type;
			int layer;
		public:
			// 通过 DataHandler 继承
			void OnRealmEnter(mule::Data::Basic::Type *realm, const std::u16string &name) override;
			void OnRealmExit(mule::Data::Basic::Type *realm, const std::u16string &name) override;
			void OnRealmEnter(mule::Data::Basic::Type *realm, int idx) override;
			void OnRealmExit(mule::Data::Basic::Type *realm, int idx) override;
			void OnSheetReadStart() override;
			void OnSheetReadEnd() override;
			void OnDataRead(const mule::Data::Basic::MultiValue &value) override;
		};
	}
}
