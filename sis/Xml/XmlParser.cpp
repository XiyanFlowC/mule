#include "XmlParser.h"

namespace mule
{
    namespace Xml
    {
        // utf8 �ػ�
        template <>
        std::string to_utf<char>(long value) {
            return xybase::string::to_utf8(value);
        }

        // utf16 �ػ�
        template <>
        std::u16string to_utf<char16_t>(long value) {
            return xybase::string::to_utf16(value);
        }

        // utf32 �ػ�
        template <>
        std::u32string to_utf<char32_t>(long value) {
            return xybase::string::to_utf32(value);
        }
    }
}
