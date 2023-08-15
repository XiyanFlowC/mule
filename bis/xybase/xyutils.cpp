#include "xyutils.h"
#include <cstdint>

std::string xybase::string::to_utf8(long codepoint)
{
    std::string ret;

    if (codepoint <= 0x7F) {
        ret += static_cast<char>(codepoint);
    }
    else if (codepoint <= 0x07FF) {
        ret += static_cast<char>((codepoint >> 6) | 0xC0);
        ret += static_cast<char>((codepoint & 0x3F) | 0x80);
    }
    else if (codepoint <= 0xFFFF) {
        ret += static_cast<char>((codepoint >> 12) | 0xE0);
        ret += static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80);
        ret += static_cast<char>((codepoint & 0x3F) | 0x80);
    }
    else if (codepoint <= 0x10FFFF) {
        ret += static_cast<char>((codepoint >> 18) | 0xF0);
        ret += static_cast<char>(((codepoint >> 12) & 0x3F) | 0x80);
        ret += static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80);
        ret += static_cast<char>((codepoint & 0x3F) | 0x80);
    }
    else {
        // Invalid codepoint
        return "";
    }

    return ret;
}

std::u16string xybase::string::to_utf16(long codepoint)
{
    std::u16string ret;

    if (codepoint <= 0xFFFF) {
        // 码点在基本平面（BMP）范围内，直接转换为一个UTF-16字符
        ret.push_back(static_cast<wchar_t>(codepoint));
    }
    else if (codepoint <= 0x10FFFF) {
        // 码点在增补平面范围内，进行UTF-16双字节编码
        codepoint -= 0x10000;
        wchar_t leadSurrogate = static_cast<wchar_t>((codepoint >> 10) + 0xD800);
        wchar_t trailSurrogate = static_cast<wchar_t>((codepoint & 0x3FF) + 0xDC00);
        ret.push_back(leadSurrogate);
        ret.push_back(trailSurrogate);
    }

    return ret;
}

std::u32string xybase::string::to_utf32(long codepoint)
{
    return std::u32string() + static_cast<char32_t>(codepoint);
}

long xybase::string::to_codepoint(std::string str)
{
    uint32_t ret = 0;
    int leng = 0;

    if (str[0] & 0x80) {
        if ((str[0] & 0xE0) == 0xC0) {
            leng = 2;
            ret = str[0] & 0x1F;
        }
        else if ((str[0] & 0xF0) == 0xE0) {
            leng = 3;
            ret = str[0] & 0x0F;
        }
        else if ((str[0] & 0xF8) == 0xF0) {
            leng = 4;
            ret = str[0] & 0x07;
        }
        else {
            // Invalid UTF-8 encoding
            return 0;
        }

        for (int i = 1; i < leng; ++i) {
            if ((str[i] & 0xC0) == 0x80) {
                ret = (ret << 6) | (str[i] & 0x3F);
            }
            else {
                // Invalid UTF-8 encoding
                return 0;
            }
        }
    }
    else {
        // ASCII character
        ret = str[0];
    }

    return ret;
}

long xybase::string::to_codepoint(std::u16string str)
{
    if (str[0] >= 0xD800 && str[0] <= 0xDBFF && 1 < str.length()) {
        // Surrogate pair for characters U+10000 to U+10FFFF
        char16_t leadSurrogate = str[0];
        char16_t trailSurrogate = str[1];

        // Singular lead surrogate
        if ((trailSurrogate & 0xDF00) != 0xDC00) return 0;

        return 0x10000 + ((leadSurrogate & 0x3FF) << 10) + (trailSurrogate & 0x3FF);
    }
    else {
        // Singular trailsurrogate
        if ((str[0] & 0xFC00) == 0xDC00) return 0;

        return str[0];
    }
}

std::u16string xybase::string::to_utf16(std::string str)
{
    StringBuilder<char16_t> sb;
    size_t i = 0;

    while (i < str.length()) {
        char c = str[i];

        if ((c & 0x80) == 0) {
            // ASCII
            sb += static_cast<char16_t>(c);
            ++i;
        }
        else if ((c & 0xE0) == 0xC0 && i + 1 < str.length()) {
            // 2-byte UTF-8
            char16_t highByte = static_cast<uint8_t>(c) & 0x1F;
            char16_t lowByte = static_cast<uint8_t>(str[i + 1]) & 0x3F;
            sb += static_cast<char16_t>((highByte << 6) | lowByte);
            i += 2;
        }
        else if ((c & 0xF0) == 0xE0 && i + 2 < str.length()) {
            // 3-byte UTF-8
            sb += to_utf16(to_codepoint(str.substr(i, 3)));
            i += 3;
        }
        else if ((c & 0xF8) == 0xF0 && i + 3 < str.length()) {
            // 4-byte UTF-8 (surrogate pairs)
            sb += to_utf16(to_codepoint(str.substr(i, 4)));
            i += 4;
        }
        else {
            // Invalid UTF-8 sequence, skip this byte
            ++i;
        }
    }

    return sb.ToString();
}

std::string xybase::string::to_utf8(std::u16string str)
{
    StringBuilder sb;

    for (size_t i = 0; i < str.length(); ++i) {
        char16_t c = str[i];

        if (c <= 0x7F) {
            sb += static_cast<char>(c); // 1-byte UTF-8 for ASCII characters
        }
        else if (c <= 0x07FF) {
            sb += static_cast<char>((c >> 6) | 0xC0); // First 5 bits
            sb += static_cast<char>((c & 0x3F) | 0x80); // Last 6 bits
        }
        else if (c >= 0xD800 && c <= 0xDBFF && i + 1 < str.length()) {
            // Surrogate pair for characters U+10000 to U+10FFFF
            char16_t leadSurrogate = c;
            char16_t trailSurrogate = str[i + 1];

            // Singular lead surrogate
            if ((trailSurrogate & 0xDF00) != 0xDC00) continue;

            uint32_t codepoint = 0x10000 + ((leadSurrogate & 0x3FF) << 10) + (trailSurrogate & 0x3FF);
            sb += static_cast<char>((codepoint >> 18) | 0xF0); // First 3 bits
            sb += static_cast<char>(((codepoint >> 12) & 0x3F) | 0x80); // Next 6 bits
            sb += static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80); // Next 6 bits
            sb += static_cast<char>((codepoint & 0x3F) | 0x80); // Last 6 bits

            ++i; // Skip the trail surrogate, as it's already processed
        }
        else {
            // Singular trailsurrogate
            if ((c & 0xFC00) == 0xDC00) continue;

            sb += to_utf8(to_codepoint(str.substr(i, 2)));
        }
    }

    return sb.ToString();
}

int xybase::io::access(const char *path, AccessMode mode)
{
#ifdef _WIN32
    int mode_ = 0;
    if (mode & PM_READ) mode_ |= 0x2;
    if (mode & PM_WRITE) mode_ |= 0x4;
    return ::_access(path, mode_);
#else
    int mode_ = 0;
    if (mode & PM_EXECUTE) mode_ |= 0111;
    if (mode & PM_READ) mode_ |= 0222;
    if (mode & PM_WRITE) mode_ |= 0444;
    return ::access(path, mode_);
#endif
}

int xybase::io::mkdir(const char *path)
{
#ifdef _WIN32
    return ::_mkdir(path);
#else
    return ::mkdir(path, 0777);
#endif
}
