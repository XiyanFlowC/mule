#include <Data/Basic/BasicType.h>
#include <Data/TypeCreator.h>

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			class Enum : public BasicType
			{
				BasicType *baseType;
				std::u16string name;
				std::map<std::u16string, MultiValue> mapS2V;
				std::map<MultiValue, std::u16string> mapV2S;
			public:
				Enum(const std::u16string &name, const std::u16string &baseType = u"int32");

				void AddEnum(std::u16string name, MultiValue value);

				size_t Size() const override;
				std::u16string GetDataType() const override;
			protected:
				MultiValue DoRead(xybase::Stream *stream) override;
				void DoWrite(xybase::Stream *stream, const MultiValue &value) override;
			};
		}
	}
}
