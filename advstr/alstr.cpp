#include "alstr.h"

#include <xyutils.h>
#include <xystring.h>
#include <VirtualFileSystem.h>
#include <Exception/NotImplementedException.h>

int str_ex(xybase::Stream *input, xybase::TextStream *output, int begin, int end)
{
	const int step = 8, threshold = 4;
	input->Seek(begin, xybase::Stream::SM_BEGIN);
	while (input->Tell() < end)
	{
		auto str = input->ReadString();
		auto addr = input->Tell();
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

		output->Write(std::format("{:08X},{},{}\n\n", addr, str.size(), (const char *)u8str.c_str()));

		input->Seek(XY_ALIGN(input->Tell(), step), xybase::Stream::SM_BEGIN);
	}
	return 0;
}

int str_im(xybase::TextStream *input, xybase::Stream *output)
{
	mule::Logger logger{ "<fn_str_im>", mule::LoggerConfig::GetLogLevel() };
	std::string line;
	auto &&stream = input->stream;
	stream.imbue(std::locale{});
	while (std::getline(stream, line))
	{
		line = line.substr(line.find_first_not_of(" \t"));
		if (line[0] == '#') continue;
		if (line.empty()) continue;
		size_t firstComma = line.find(',');
		int addr = xybase::string::stoi(line.substr(0, firstComma), 16);
		int size = xybase::string::stoi(line.substr(firstComma + 1, line.find(',', firstComma + 1) - firstComma - 1));
		// Beware the str is in utf-8!!!
		std::string str = line.substr(line.find(',', firstComma + 1));
		// HACK: The utf-8 string stored in the std::string can be operate as this to 'convert' to the std::u8string (with constructor
		// note that might violate the type ? I have no idea how to convert the type from the ifstream anyway
		std::string rawStr = xybase::string::to_string((char8_t *)(str.c_str()));
		if (rawStr.size() > size)
		{
			logger.Error(L"String size ({}) exceeded the limitation ({})!", size, rawStr.size());
			logger.Note(L"When processing input file {}.", xybase::string::to_wstring(input->GetName()));
			logger.Note(L"When patching output file {}.", xybase::string::to_wstring(input->GetName()));
			logger.Note(L"String={}", xybase::string::to_wstring(rawStr));
			continue;
		}
		output->Seek(addr, xybase::Stream::SM_BEGIN);
		output->Write(rawStr);
	}
	
	return 0;
}
