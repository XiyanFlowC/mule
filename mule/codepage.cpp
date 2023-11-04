#include "codepage.h"
#include <xystring.h>
#include <StringBuilder.h>

std::string cvt_to_string(const std::wstring &str)
{
	return CodeCvt::GetInstance().CvtToString(str);
}

std::wstring cvt_to_wstring(const std::string &str)
{
	return CodeCvt::GetInstance().CvtToWString(str);
}

#pragma pack(push,1)
struct CodePageEntry
{
	uint32_t dbc;
	uint32_t wc;
};
#pragma pack(pop)

CodeCvt::~CodeCvt()
{
	xybase::string::set_string_cvt(nullptr, nullptr);
}

CodeCvt &CodeCvt::GetInstance()
{
	static CodeCvt _inst;
	return _inst;
}

std::string CodeCvt::CvtToString(const std::wstring &str)
{
	auto codes = xybase::string::to_utf32(str);

	xybase::StringBuilder<char> sb;
	for (char32_t code : str)
	{
		auto dbc = uc2cp[code];
		if (dbc == 0)
		{
			dbc = uc2cp[U'〓'];
			if (dbc == 0)
			{
				dbc = uc2cp[U'?'];
			}
		}
		if (dbc > 0xFF)
		{
			sb.Append((dbc & 0xFF00) >> 8);
			sb.Append(dbc & 0xFF);
		}
		else
		{
			sb.Append(dbc);
		}
	}

	return sb.ToString();
}

std::wstring CodeCvt::CvtToWString(const std::string &str)
{
	xybase::StringBuilder<char32_t> sb;
	int current = 0;
	for (char ch : str)
	{
		if (current)
		{
			auto wc = cp2uc[current | ch];
			if (wc == 0)
			{
				wc = U'�';
			}

			sb.Append(wc);
			current = 0;
		}
		else
		{
			if (ch & 0x80)
			{
				current = ch << 8;
			}
			else
			{
				auto wc = cp2uc[ch];
				if (wc == 0)
				{
					wc = U'�';
				}

				sb.Append(wc);
			}
		}
	}
	return xybase::string::to_wstring(sb.ToString());
}

void CodeCvt::Init(const char *cp)
{
	uc2cp.clear();
	cp2uc.clear();
	uint64_t count = *((uint64_t *)cp);
	if (count & 3) abort();
	count >>= 3;
	CodePageEntry *cpe = (CodePageEntry *)(cp + sizeof(uint64_t));
	for (uint64_t i = 0; i < count; ++i)
	{
		if (!uc2cp.contains(cpe[i].wc))
			uc2cp[cpe[i].wc] = cpe[i].dbc;
		cp2uc[cpe[i].dbc] = cpe[i].wc;
	}

	xybase::string::set_string_cvt(cvt_to_wstring, cvt_to_string);
}
