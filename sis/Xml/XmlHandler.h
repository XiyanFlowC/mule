#pragma once

#ifndef XML_HANDLER_H__
#define XML_HANDLER_H__

#include <Data/Basic/Object.h>

namespace mule
{
	namespace Xml
	{
		class XmlHandler : public mule::Data::Basic::Object::DataHandler
		{
			virtual void OnRealmEnter(mule::Data::Basic::Object *realm, std::string name) override;
			virtual void OnRealmExit(mule::Data::Basic::Object *realm, std::string name) override;
			virtual void OnRealmEnter(mule::Data::Basic::Object *realm, int idx) override;
			virtual void OnRealmExit(mule::Data::Basic::Object *realm, int idx) override;
			virtual void OnDataRead(const mule::Data::Basic::MultiValue &value) override;
			virtual mule::Data::Basic::MultiValue OnDataWrite() override;
		};
	}
}

#endif /* End of XML_HANDLER_H__*/
