#pragma once

#ifndef XML_HANDLER_H__
#define XML_HANDLER_H__

#include <TextStream.h>
#include <Data/Basic/Type.h>

#include "XmlNode.h"
#include "XmlParser.h"

namespace mule
{
	namespace Xml
	{
		class XmlHandler : public mule::Data::Basic::Type::DataHandler, public mule::Data::Basic::Type::FileHandler
		{
		public:
			XmlHandler();

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

			virtual void OnSheetWriteStart() override;

			virtual void OnSheetWriteEnd() override;

			virtual void OnRealmEnter(mule::Data::Basic::Type *realm, const std::u16string &name) override;
			virtual void OnRealmExit(mule::Data::Basic::Type *realm, const std::u16string &name) override;
			virtual void OnRealmEnter(mule::Data::Basic::Type *realm, int idx) override;
			virtual void OnRealmExit(mule::Data::Basic::Type *realm, int idx) override;
			virtual void OnDataRead(const mule::Data::Basic::MultiValue &value) override;
			virtual const mule::Data::Basic::MultiValue OnDataWrite() override;

			virtual void AppendMetadatum(std::u16string name, const mule::Data::Basic::MultiValue &mv) override;
		protected:
			mule::Xml::XmlParser<mule::Xml::XmlNode, char8_t> xmlParser;

		private:
			void ReadTagAndParse(const std::u8string &tagName, xybase::StringBuilder<char8_t> &sb, bool isString);
			enum {
				XHS_IDLE,
				XHS_READ,
				XHS_WRITE,
			} status;

			mule::Data::Basic::MultiValue element;

			std::u16string nodeName;

			int layer = 0;
		};
	}
}

#endif /* End of XML_HANDLER_H__*/
