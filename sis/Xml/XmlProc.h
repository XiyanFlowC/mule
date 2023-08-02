#pragma once

#ifndef XML_PROCESSOR_H__
#define XML_PROCESSOR_H__

#include <tuple>
#include <map>
#include <Data/Basic/MultiValue.h>
#include <functional>

namespace mule
{
	namespace Xml
	{
		/**
		 * @brief 处理Xml的类。
		*/
		class XmlParser
		{
        public:
			XmlParser();

            mule::Data::Basic::MultiValue Parse(const std::string &xml);

            void RegisterTagCallback(const std::string &tagName, std::function<std::string(mule::Data::Basic::MultiValue &)> callback);

			void RegisterEntity(const std::string entityName, const std::string entitySeq);

			std::string error;

        private:
            std::tuple<std::string, mule::Data::Basic::MultiValue> ParseNode(const std::string &xml);

            size_t index;

            std::map<std::string, std::function<std::string(mule::Data::Basic::MultiValue &)>> callbacks;

			std::map<std::string, std::string> entities;
		};

		class XmlGenerator
		{
		public:
			XmlGenerator();

			std::string ToXml(const mule::Data::Basic::MultiValue &value, std::string rootName = "root");

			std::function<std::string(std::string)> callback;
		private:
			int layer;

			static int ident;

			void ProcNode(const mule::Data::Basic::MultiValue &value, xybase::StringBuilder<char> &sb);
		};
	}
}

#endif // !XML_PROCESSOR_H__
