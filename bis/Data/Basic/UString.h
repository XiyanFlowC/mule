#pragma once

#ifndef USTRING_OBJECT_H__
#define USTRING_OBJECT_H__

#include <Data/Basic/BasicType.h>
#include <Logger.h>
#include <xystring.h>

namespace mule
{
	namespace Data
	{
		namespace Basic
		{

			/**
			 * @brief String is not a typical object cause it holds a variable length.
			*/
			template<class UType>
			class UString : public BasicType
			{
				Logger logger = Logger::GetLogger<UString<UType>>();
			protected:
				size_t lastSize = -1;
			public:
				size_t Size() const override;
				std::u16string GetDataType() const override;

				size_t GetLastSize() const override;
				size_t EvalSize(const MultiValue &obj) const override;
			protected:
				MultiValue DoRead(xybase::Stream *stream) override;
				void DoWrite(xybase::Stream *stream, const MultiValue &value) override;
			};
			template<class UType>
			inline size_t UString<UType>::Size() const
			{
				return -1;
			}
			template<class UType>
			inline std::u16string UString<UType>::GetDataType() const
			{
				return u"string/text";
			}
			template<class UType>
			inline size_t UString<UType>::GetLastSize() const
			{
				int factor = 1;
				if constexpr (std::is_same_v<UType, char8_t>) factor = 1;
				else if constexpr (std::is_same_v<UType, char16_t>) factor = 2;
				else if constexpr (std::is_same_v<UType, char32_t>) factor = 4;
				return (lastSize + 1) * factor;
			}
			template<class UType>
			inline size_t UString<UType>::EvalSize(const MultiValue &obj) const
			{
				int factor = 1;
				if constexpr (std::is_same_v<UType, char8_t>) factor = 1;
				else if constexpr (std::is_same_v<UType, char16_t>) factor = 2;
				else if constexpr (std::is_same_v<UType, char32_t>) factor = 4;
				
				auto encStr = xybase::string::to_enc<UType, char16_t>(*obj.value.stringValue);
				return (encStr.size() + 1) * factor;
			}
			template<class UType>
			inline MultiValue UString<UType>::DoRead(xybase::Stream *stream)
			{
				xybase::StringBuilder<UType> sb;
				UType ch = 0;

				if constexpr (std::is_same_v<UType, char16_t>)
					ch = stream->ReadUInt16();
				else if constexpr (std::is_same_v<UType, char8_t>)
					ch = stream->ReadUInt8();
				else if constexpr (std::is_same_v<UType, char32_t>)
					ch = stream->ReadUInt32();

				lastSize = 0;
				while (ch)
				{
					sb += ch;
					lastSize++;

					if constexpr (std::is_same_v<UType, char16_t>)
						ch = stream->ReadUInt16();
					else if constexpr (std::is_same_v<UType, char8_t>)
						ch = stream->ReadUInt8();
					else if constexpr (std::is_same_v<UType, char32_t>)
						ch = stream->ReadUInt32();
				}
				MultiValue tmp{ xybase::string::to_enc<char16_t, UType>(sb.ToString()) };
				tmp.metadata[u"size"] = lastSize;
				return tmp;
			}
			template<class UType>
			inline void UString<UType>::DoWrite(xybase::Stream *stream, const MultiValue &value)
			{
				if (!value.IsType(MultiValue::MVT_STRING)) throw xybase::InvalidParameterException(L"value", L"Type mismatch!", 19604);

				auto data = xybase::string::to_enc<UType, char16_t>(*(value.value.stringValue));

				auto &&itr = value.metadata.find(u"size");
				if (itr != value.metadata.end())
				{
					if (data.size() > itr->second.value.unsignedValue)
						throw xybase::InvalidParameterException(
							L"value",
							std::format(
								L"Too large (size {} exceeded {}), string={}",
								data.size(),
								itr->second.value.unsignedValue,
								xybase::string::to_wstring(*value.value.stringValue)),
							19610);
				}

				if (value.value.stringValue->size() && data.size() == 0)
				{
					logger.Warn(L"Failed to encode string.");
					logger.Note(L"String={}", xybase::string::to_wstring(*value.value.stringValue));
				}


				int factor = 1;
				if constexpr (std::is_same_v<UType, char8_t>) factor = 1;
				else if constexpr (std::is_same_v<UType, char16_t>) factor = 2;
				else if constexpr (std::is_same_v<UType, char32_t>) factor = 4;
				stream->Write((char *)data.c_str(), (data.size() + 1) * factor);
				lastSize = data.size();
			}
		}
	}
}

#endif
