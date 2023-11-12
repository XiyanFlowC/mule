#include "advstr.h"

#include <xystring.h>
#include <cstring>
#include "alstr.h"

int b2t(const char16_t *converter, xybase::Stream *input, xybase::TextStream *output, const char16_t *param)
{
	if (0 == memcmp(u"addrtxt", converter, sizeof(u"addrtxt")))
	{
		std::u16string para(param);
		std::u16string sta = para.substr(0, para.find(':')),
			end = para.substr(para.find(':') + 1);
		return str_ex(input, output, xybase::string::stoi(sta, 16), xybase::string::stoi(end, 16));
	}
	return -1;
}

int t2b(const char16_t *converter, xybase::TextStream *input, xybase::Stream *output, const char16_t *param)
{
	if (0 == memcmp(u"addrtxt", converter, sizeof(u"addrtxt")))
	{
		return str_im(input, output);
	}
	return -1;
}

mule::PluginDescription desc{
	.name = L"Advanced String Operations",
	.author = L"xiyan",
	.licence = L"MIT",
	.majorVer = 1,
	.minorVer = 4,
	.description = L"Provides advanced string operation feature",
	.ConvertToText = b2t,
	.ConvertToBinary = t2b
};
