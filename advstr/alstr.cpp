#include "alstr.h"

#include <xyutils.h>
#include <xystring.h>
#include <VirtualFileSystem.h>
#include <Exception/NotImplementedException.h>
#include <Configuration.h>

int str_ex(xybase::Stream *input, xybase::TextStream *output, int begin, int end)
{
	int step = (int)mule::Configuration::GetInstance().GetSigned(u"addrtext.step", 8),
		threshold = (int)mule::Configuration::GetInstance().GetSigned(u"addrtext.threshold", 4);
	input->Seek(begin, xybase::Stream::SM_BEGIN);
	while (input->Tell() < end)
	{
		auto addr = input->Tell();
		auto str = input->ReadString();
		if (str.size() < threshold)
		{
			input->Seek(XY_ALIGN(input->Tell(), step), xybase::Stream::SM_BEGIN);
			continue;
		}
		auto u8str = xybase::string::to_utf8(str);
		if (u8str.size() <= 0)
		{
			input->Seek(XY_ALIGN(input->Tell(), step), xybase::Stream::SM_BEGIN);
			continue;
		}

		output->Write(std::format("{:08X},{:02},{}\n\n", addr, str.size(), (const char *)u8str.c_str()));

		input->Seek(XY_ALIGN(input->Tell(), step), xybase::Stream::SM_BEGIN);
	}
	return 0;
}

int str_im(xybase::TextStream *input, xybase::Stream *output)
{
	mule::Logger logger{ "<fn_str_im>" };
	std::string line;
	while (input->ReadLine(line))
	{
		if (line.empty()) continue;
		line = line.substr(line.find_first_not_of(" \t"));
		if (line[0] == '#') continue;
		size_t firstComma = line.find(',');
		int addr = (int)xybase::string::stoi(line.substr(0, firstComma), 16);
		int size = (int)xybase::string::stoi(line.substr(firstComma + 1, line.find(',', firstComma + 1) - firstComma - 1));
		// Beware the str is in utf-8!!!
		std::string str = line.substr(line.find(',', firstComma + 1) + 1);
		// HACK: The utf-8 string stored in the std::string can be operate as this to 'convert' to the std::u8string (with constructor
		// note that might violate the type ? I have no idea how to convert the type from the ifstream anyway
		std::string rawStr = xybase::string::to_string((char8_t *)(str.c_str()));
		if (rawStr.size() > size)
		{
			logger.Error(L"String size ({}) exceeded the limitation ({})!", rawStr.size(), size);
			logger.Note(L"When processing input file {}.", xybase::string::to_wstring(input->GetName()));
			logger.Note(L"When patching output file {}.", xybase::string::to_wstring(output->GetName()));
			logger.Note(L"Position={}", addr);
			continue;
		}
		output->Seek(addr, xybase::Stream::SM_BEGIN);
		output->Write(rawStr);
	}
	
	return 0;
}
