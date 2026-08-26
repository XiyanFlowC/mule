#pragma once

#ifndef XML_HANDLER_H__
#define XML_HANDLER_H__

#include <TextStream.h>
#include <Data/Basic/Type.h>

#include <Xml/XmlParser.h>

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
			xybase::xml::XmlParser<xybase::xml::XmlNode, char8_t> xmlParser;

		private:
			void ReadTagAndParse(const std::u8string &tagName, std::u8string &frag, bool isString, bool isText);
			enum {
				XHS_IDLE,
				XHS_READ,
				XHS_READ_METADATA_WAITING,
				XHS_WRITE,
			} status;

			mule::Data::Basic::MultiValue element;

			void SkipComment(char chAfterLt);

			/// 在标签名读取完成后，消费开标签剩余部分（空白、属性、'>' 或 '/>'）。
			/// ch 是标签名后的第一个字符；openTag 初始为（不含 '<' 的）标签名。
			/// 把属性写入 element.metadata；把一个完整开标签（终止于 '>' 或 '/>'）累加到 openTag。
			/// 返回该标签是否自闭合（以 '/>' 结束）。
			bool ConsumeOpenTagTail(char &ch, std::u8string &openTag);

			std::u16string nodeName;

			int layer = 0;
		};
	}
}

#endif /* End of XML_HANDLER_H__*/
