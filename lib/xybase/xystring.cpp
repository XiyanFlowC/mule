#include "xystring.h"
#include <locale>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include "StringBuilder.h"

std::u8string xybase::string::to_utf8(long codepoint)
{
    std::u8string ret;

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
        return u8"";
    }

    return ret;
}

std::u16string xybase::string::to_utf16(long codepoint)
{
    std::u16string ret;

    if (codepoint <= 0xFFFF) {
        // 码点在基本平面（BMP）范围内，直接转换为一个UTF-16字符
        ret.push_back(static_cast<char16_t>(codepoint));
    }
    else if (codepoint <= 0x10FFFF) {
        // 码点在增补平面范围内，进行UTF-16双字节编码
        codepoint -= 0x10000;
        char16_t leadSurrogate = static_cast<char16_t>((codepoint >> 10) + 0xD800);
        char16_t trailSurrogate = static_cast<char16_t>((codepoint & 0x3FF) + 0xDC00);
        ret.push_back(leadSurrogate);
        ret.push_back(trailSurrogate);
    }

    return ret;
}

std::u32string xybase::string::to_utf32(long codepoint)
{
    return std::u32string() + static_cast<char32_t>(codepoint);
}

long xybase::string::to_codepoint(const std::u8string &str)
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

long xybase::string::to_codepoint(const std::u16string &str)
{
    if (str[0] >= 0xD800 && str[0] <= 0xDBFF && 1 < str.length()) {
        // Surrogate pair for characters U+10000 to U+10FFFF
        char16_t leadSurrogate = str[0];
        char16_t trailSurrogate = str[1];

        // Singular lead surrogate
        if ((trailSurrogate & 0xFC00) != 0xDC00) return 0xFFFD;

        return 0x10000 + ((leadSurrogate & 0x3FF) << 10) + (trailSurrogate & 0x3FF);
    }
    else {
        // Singular trailsurrogate
        if ((str[0] & 0xFC00) == 0xDC00) return 0xFFFD;

        return str[0];
    }
}

std::u32string xybase::string::to_utf32(const std::string &str) noexcept
{
    return to_utf32(to_wstring(str));
}

std::u32string xybase::string::to_utf32(const std::u8string &str) noexcept
{
    StringBuilder<char32_t> sb;
    size_t i = 0;

    while (i < str.length()) {
        uint32_t res = 0;
        int leng = 1;

        if (str[i] & 0x80) {
            if ((str[i] & 0xE0) == 0xC0) {
                leng = 2;
                res = str[i] & 0x1F;
            }
            else if ((str[i] & 0xF0) == 0xE0) {
                leng = 3;
                res = str[i] & 0x0F;
            }
            else if ((str[i] & 0xF8) == 0xF0) {
                leng = 4;
                res = str[i] & 0x07;
            }
            else {
                // Invalid UTF-8 encoding
                sb += u'�';
                i += 1;
                continue;
            }

            if (i + leng > str.size())
            {
                sb += u'�';
                break;
            }

            for (int j = 1; j < leng; ++j) {
                if ((str[i + j] & 0xC0) == 0x80) {
                    res = (res << 6) | (str[i + j] & 0x3F);
                }
                else {
                    // Invalid UTF-8 encoding
                    res = u'�';
                    break;
                }
            }
        }
        else {
            // ASCII character
            res = str[i];
        }

        sb += res;
        i += leng;
    }

    return sb.ToString();
}

std::u32string xybase::string::to_utf32(const std::u16string &str) noexcept
{
    StringBuilder<char32_t> sb;
    size_t i = 0;

    while (i < str.size())
    {
        if (str[i] >= 0xD800 && str[i] <= 0xDBFF) {
            // Singular lead surrogate
            if (i + 1 < str.length())
            {
                sb += u'�';
                break;
            }

            // Surrogate pair for characters U+10000 to U+10FFFF
            char16_t leadSurrogate = str[i++];
            char16_t trailSurrogate = str[i++];

            // Singular lead surrogate
            if ((trailSurrogate & 0xFC00) != 0xDC00)
            {
                sb += u'�';
                sb += trailSurrogate;
                continue;
            }

            sb += static_cast<char32_t>(0x10000 + ((leadSurrogate & 0x3FF) << 10) + (trailSurrogate & 0x3FF));
        }
        else {
            // Singular trailsurrogate
            if ((str[i] & 0xFC00) == 0xDC00)
            {
                sb += u'�';
                continue;
            }

            sb += str[i++];
        }
    }

    return sb.ToString();
}

std::u32string xybase::string::to_utf32(const std::u32string &str) noexcept
{
    return str;
}

std::u32string xybase::string::to_utf32(const std::wstring &str) noexcept
{
#ifdef _WIN32
    return to_utf32((const char16_t*)str.c_str());
#else
    return std::u32string((const char32_t*)str.c_str());
#endif
}

std::u16string xybase::string::to_utf16(const std::string &str) noexcept
{
    return to_utf16(to_wstring(str));
}

std::u16string xybase::string::to_utf16(const std::u8string &str) noexcept
{
    StringBuilder<char16_t> sb;
    size_t i = 0;

    while (i < str.length()) {
        char8_t c = str[i];

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

std::u16string xybase::string::to_utf16(const std::u16string &str) noexcept
{
    return str;
}

std::u16string xybase::string::to_utf16(const std::u32string &str) noexcept
{
    std::u16string ret;
    for (auto ch : str)
    {
        if (ch <= 0xFFFF) {
            // 码点在基本平面（BMP）范围内，直接转换为一个UTF-16字符
            ret.push_back(static_cast<char16_t>(ch));
        }
        else if (ch <= 0x10FFFF) {
            // 码点在增补平面范围内，进行UTF-16双字节编码
            ch -= 0x10000;
            char16_t leadSurrogate = static_cast<char16_t>((ch >> 10) + 0xD800);
            char16_t trailSurrogate = static_cast<char16_t>((ch & 0x3FF) + 0xDC00);
            ret.push_back(leadSurrogate);
            ret.push_back(trailSurrogate);
        }
    }
    return ret;
}

std::u16string xybase::string::to_utf16(const std::wstring &str) noexcept
{
#ifdef _WIN32
    return std::u16string((const char16_t *)str.c_str());
#else
    return to_utf16((const char32_t *)str.c_str());
#endif // _WIN32
}

std::u8string xybase::string::to_utf8(const std::wstring &str) noexcept
{
    return to_utf8(to_utf16(str));
}

std::u8string xybase::string::to_utf8(const std::u32string &str) noexcept
{
    StringBuilder<char8_t> sb;
    for (const char32_t &ch : str)
    {
        if (ch <= 0x7F) {
            sb += static_cast<char>(ch);
        }
        else if (ch <= 0x07FF) {
            sb += static_cast<char>((ch >> 6) | 0xC0);
            sb += static_cast<char>((ch & 0x3F) | 0x80);
        }
        else if (ch <= 0xFFFF) {
            sb += static_cast<char>((ch >> 12) | 0xE0);
            sb += static_cast<char>(((ch >> 6) & 0x3F) | 0x80);
            sb += static_cast<char>((ch & 0x3F) | 0x80);
        }
        else if (ch <= 0x10FFFF) {
            sb += static_cast<char>((ch >> 18) | 0xF0);
            sb += static_cast<char>(((ch >> 12) & 0x3F) | 0x80);
            sb += static_cast<char>(((ch >> 6) & 0x3F) | 0x80);
            sb += static_cast<char>((ch & 0x3F) | 0x80);
        }
        else {
            sb += 0xEF;
            sb += 0xBF;
            sb += 0xBD;
            continue;
        }
    }
    return sb.ToString();
}

std::u8string xybase::string::to_utf8(const std::u16string &str) noexcept
{
    StringBuilder<char8_t> sb;

    for (size_t i = 0; i < str.length(); ++i) {
        char16_t c = str[i];

        if (c <= 0x7F) {
            sb += static_cast<char>(c); // 1-byte UTF-8 for ASCII characters
        }
        else if (c <= 0x07FF) {
            sb += static_cast<char>((c >> 6) | 0xC0); // First 5 bits
            sb += static_cast<char>((c & 0x3F) | 0x80); // Last 6 bits
        }
        else if (c >= 0xD800 && c <= 0xDBFF) {
            // Singular lead surrogate
            if (i + 1 < str.length())
            {
                sb += 0xEF;
                sb += 0xBF;
                sb += 0xBD;
                break;
            }

            // Surrogate pair for characters U+10000 to U+10FFFF
            char16_t leadSurrogate = c;
            char16_t trailSurrogate = str[i + 1];

            // Singular lead surrogate
            if ((trailSurrogate & 0xFC00) != 0xDC00)
            {
                sb += 0xEF;
                sb += 0xBF;
                sb += 0xBD;
                continue;
            }

            uint32_t codepoint = 0x10000 + ((leadSurrogate & 0x3FF) << 10) + (trailSurrogate & 0x3FF);
            sb += static_cast<char>((codepoint >> 18) | 0xF0); // First 3 bits
            sb += static_cast<char>(((codepoint >> 12) & 0x3F) | 0x80); // Next 6 bits
            sb += static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80); // Next 6 bits
            sb += static_cast<char>((codepoint & 0x3F) | 0x80); // Last 6 bits

            ++i; // Skip the trail surrogate, as it's already processed
        }
        else {
            // Singular trail surrogate
            if ((c & 0xFC00) == 0xDC00)
            {
                sb += 0xEF;
                sb += 0xBF;
                sb += 0xBD;
                continue;
            }

            sb += to_utf8(to_codepoint(str.substr(i, 2)));
        }
    }

    return sb.ToString();
}

std::u8string xybase::string::to_utf8(const std::u8string &str) noexcept
{
    return str;
}

std::u8string xybase::string::to_utf8(const std::string &str) noexcept
{
    return to_utf8(to_wstring(str));
}

std::wstring xybase::string::to_wstring(const std::u8string &str) noexcept
{
#ifdef _WIN32
    return std::wstring((const wchar_t *)to_utf16(str).c_str());
#else
    return std::wstring((const wchar_t *)to_utf32(str).c_str());
#endif // _WIN32
}

std::wstring xybase::string::to_wstring(const std::u16string &str) noexcept
{
#ifdef _WIN32
    return std::wstring((const wchar_t *)to_utf16(str).c_str());
#else
    return std::wstring((const wchar_t *)to_utf32(str).c_str());
#endif // _WIN32
}

std::wstring xybase::string::to_wstring(const std::u32string &str) noexcept
{
#ifdef _WIN32
    return std::wstring((const wchar_t *)to_utf16(str).c_str());
#else
    return std::wstring((const wchar_t *)to_utf32(str).c_str());
#endif // _WIN32
}

std::wstring xybase::string::to_wstring(const std::wstring &str) noexcept
{
    return str;
}

std::string xybase::string::to_string(const std::string &str) noexcept
{
    return str;
}

std::string xybase::string::to_string(const std::u8string &str) noexcept
{
    return to_string(to_wstring(str));
}

std::string xybase::string::to_string(const std::u16string &str) noexcept
{
    return to_string(to_wstring(str));
}

std::string xybase::string::to_string(const std::u32string &str) noexcept
{
    return to_string(to_wstring(str));
}

static std::wstring(*fcn_mbcstowcs)(const std::string &) = nullptr;
static std::string(*fcn_wcstombcs)(const std::wstring &) = nullptr;

void xybase::string::set_string_cvt(std::wstring(*p_mbcstowcs)(const std::string &), std::string(*p_wcstombcs)(const std::wstring &)) noexcept
{
    fcn_mbcstowcs = p_mbcstowcs;
    fcn_wcstombcs = p_wcstombcs;
}

std::wstring xybase::string::to_wstring(const std::string &str) noexcept
{
    if (fcn_mbcstowcs != nullptr)
    {
        return fcn_mbcstowcs(str);
    }

    return sys_mbs_to_wcs(str);
}

std::wstring xybase::string::sys_mbs_to_wcs(const std::string &str) noexcept
{
    mbstate_t state{};
    const char *pstr = str.c_str();
    size_t size = mbsrtowcs(NULL, &pstr, 0, &state);
    if (size == (size_t)-1) return L"";
    assert(mbsinit(&state));
    wchar_t *buf = new wchar_t[size + 1];
    mbsrtowcs(buf, &pstr, size + 1, &state);
    buf[size] = 0;
    std::wstring ret{ buf };
    delete[] buf;
    return ret;
}

std::string xybase::string::to_string(const std::wstring &str) noexcept
{
    if (fcn_wcstombcs != nullptr)
        return fcn_wcstombcs(str);

    return sys_wcs_to_mbs(str);
}

std::string xybase::string::sys_wcs_to_mbs(const std::wstring &str) noexcept
{
    mbstate_t state{};
    const wchar_t *pstr = str.c_str();
    size_t size = wcsrtombs(NULL, &pstr, 0, &state);
    if (size == (size_t)-1) return "";
    assert(mbsinit(&state));
    char *buf = new char[size + 1];
    wcsrtombs(buf, &pstr, size + 1, &state);
    buf[size] = 0;
    std::string ret{ buf };
    delete[] buf;
    return ret;
}
