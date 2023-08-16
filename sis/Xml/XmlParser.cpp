#include "XmlParser.h"

namespace mule
{
    namespace Xml
    {
        // utf8 特化
        template <>
        std::u8string to_utf<char8_t>(long value) {
            return xybase::string::to_utf8(value);
        }

        // utf16 特化
        template <>
        std::u16string to_utf<char16_t>(long value) {
            return xybase::string::to_utf16(value);
        }

        // utf32 特化
        template <>
        std::u32string to_utf<char32_t>(long value) {
            return xybase::string::to_utf32(value);
        }

        template <>
        std::basic_string<char8_t> str_encflip(const std::string &str)
        {
            return xybase::string::to_utf8(str);
        }

        template <>
        std::basic_string<char16_t> str_encflip(const std::string &str)
        {
            return xybase::string::to_utf16(str);
        }

        template <>
        std::basic_string<char32_t> str_encflip(const std::string &str)
        {
            return xybase::string::to_utf32(str);
        }
    }
}
