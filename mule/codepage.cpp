#include "codepage.h"
#include <map>
#include <xystring.h>
#include <StringBuilder.h>
std::map<uint32_t, uint32_t> wctodbc;
std::map<uint32_t, uint32_t> dbctowc;

std::string cvt_to_string(const std::wstring &str)
{
	auto codes = xybase::string::to_utf32(str);

	xybase::StringBuilder<char> sb;
	for (char32_t code : str)
	{
		auto dbc = wctodbc[code];
		if (dbc == 0)
		{
			dbc = wctodbc[U'〓'];
			if (dbc == 0)
			{
				dbc = wctodbc[U'?'];
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

std::wstring cvt_to_wstring(const std::string &str)
{
	xybase::StringBuilder<char32_t> sb;
	int current = 0;
	for (char ch : str)
	{
		if (current)
		{
			auto wc = dbctowc[current | ch];
			if (wc == 0)
			{
				wc = U'�';
			}

			sb.Append(wc);
			current = 0;
		}
		else
		{
			if (ch > 0x7F)
			{
				current = ch << 8;
			}
			else
			{
				auto wc = dbctowc[ch];
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

void init_codepage(const char *cp)
{
	uint64_t count = *((uint64_t *)cp);
	struct CodePageEntry { uint32_t dbc; uint32_t wc; };
	CodePageEntry *cpe = (CodePageEntry *)(cp + sizeof(uint64_t));
	for (uint64_t i = 0; i < count; ++i)
	{
		wctodbc[cpe[i].wc] = cpe[i].dbc;
		dbctowc[cpe[i].dbc] = cpe[i].wc;
	}
}
