#pragma once

#ifndef XML_HANDLER_H__
#define XML_HANDLER_H__

#include <Data/Basic/Type.h>

namespace mule
{
	namespace Xml
	{
		class XmlHandler : public mule::Data::Basic::Type::DataHandler
		{
		public:
			virtual void OnRealmEnter(mule::Data::Basic::Type *realm, std::u16string name) override;
			virtual void OnRealmExit(mule::Data::Basic::Type *realm, std::u16string name) override;
			virtual void OnRealmEnter(mule::Data::Basic::Type *realm, int idx) override;
			virtual void OnRealmExit(mule::Data::Basic::Type *realm, int idx) override;
			virtual void OnDataRead(const mule::Data::Basic::MultiValue &value) override;
			virtual mule::Data::Basic::MultiValue OnDataWrite() override;
		private:
			enum {
				DHXHS_IDLE,
				DHXHS_READ,
				DHXHS_WRITE
			} status;
		};
	}
}

#endif /* End of XML_HANDLER_H__*/
