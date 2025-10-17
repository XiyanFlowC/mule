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
	int lineNumber = 0;
	
	while (input->ReadLine(line))
	{
		lineNumber++;
		
		try
		{
			if (line.empty()) continue;
			line = line.substr(line.find_first_not_of(" \t"));
			if (line.empty()) continue;
			if (line[0] == '#') continue;
			
			size_t firstComma = line.find(',');
			if (firstComma == std::string::npos)
			{
				logger.Error(L"Invalid line format: no comma found at line {}.", lineNumber);
				// 正确转换 UTF-8 编码的 line 到 wstring
				std::u8string utf8Line(reinterpret_cast<const char8_t*>(line.c_str()), line.length());
				logger.Note(L"Line content: {}", xybase::string::to_wstring(utf8Line));
				continue;
			}
			
			size_t secondComma = line.find(',', firstComma + 1);
			if (secondComma == std::string::npos)
			{
				logger.Error(L"Invalid line format: second comma not found at line {}.", lineNumber);
				std::u8string utf8Line(reinterpret_cast<const char8_t*>(line.c_str()), line.length());
				logger.Note(L"Line content: {}", xybase::string::to_wstring(utf8Line));
				continue;
			}
			
			// 解析地址
			int addr = 0;
			try
			{
				std::string addrStr = line.substr(0, firstComma);
				if (addrStr.empty())
				{
					logger.Error(L"Empty address field at line {}.", lineNumber);
					continue;
				}
				addr = (int)xybase::string::stoi(addrStr, 16);
			}
			catch (const std::invalid_argument& e)
			{
				logger.Error(L"Invalid address format at line {}: cannot convert to hexadecimal integer.", lineNumber);
				std::u8string utf8Line(reinterpret_cast<const char8_t*>(line.c_str()), line.length());
				logger.Note(L"Line content: {}", xybase::string::to_wstring(utf8Line));
				continue;
			}
			catch (const std::out_of_range& e)
			{
				logger.Error(L"Address value out of range at line {}.", lineNumber);
				std::u8string utf8Line(reinterpret_cast<const char8_t*>(line.c_str()), line.length());
				logger.Note(L"Line content: {}", xybase::string::to_wstring(utf8Line));
				continue;
			}
			
			// 解析大小
			int size = 0;
			try
			{
				std::string sizeStr = line.substr(firstComma + 1, secondComma - firstComma - 1);
				if (sizeStr.empty())
				{
					logger.Error(L"Empty size field at line {}.", lineNumber);
					continue;
				}
				size = (int)xybase::string::stoi(sizeStr);
			}
			catch (const std::invalid_argument& e)
			{
				logger.Error(L"Invalid size format at line {}: cannot convert to integer.", lineNumber);
				std::u8string utf8Line(reinterpret_cast<const char8_t*>(line.c_str()), line.length());
				logger.Note(L"Line content: {}", xybase::string::to_wstring(utf8Line));
				continue;
			}
			catch (const std::out_of_range& e)
			{
				logger.Error(L"Size value out of range at line {}.", lineNumber);
				std::u8string utf8Line(reinterpret_cast<const char8_t*>(line.c_str()), line.length());
				logger.Note(L"Line content: {}", xybase::string::to_wstring(utf8Line));
				continue;
			}
			
			// 提取字符串部分 (UTF-8)
			std::string str;
			try
			{
				str = line.substr(secondComma + 1);
			}
			catch (const std::out_of_range& e)
			{
				logger.Error(L"Failed to extract string content at line {}.", lineNumber);
				std::u8string utf8Line(reinterpret_cast<const char8_t*>(line.c_str()), line.length());
				logger.Note(L"Line content: {}", xybase::string::to_wstring(utf8Line));
				continue;
			}
			
			// 处理转义字符
			xybase::StringBuilder<char8_t> sb;
			int escaped = 0, xch = 0;
			
			try
			{
				for (const char &ch : str)
				{
					if (escaped)
					{
						if (escaped == 2)
						{
							++escaped;
							try
							{
								xch = (int)xybase::string::stoi<char>({ ch }, 16) << 8;
							}
							catch (const std::invalid_argument& e)
							{
								logger.Warn(L"Invalid hex escape sequence at line {}, treating as literal character.", lineNumber);
								sb += static_cast<char8_t>(ch);
								escaped = 0;
								continue;
							}
						}
						else if (escaped == 3)
						{
							escaped = 0;
							try
							{
								xch |= (int)xybase::string::stoi<char>({ ch }, 16);
								sb += static_cast<char8_t>(xch);
							}
							catch (const std::invalid_argument& e)
							{
								logger.Warn(L"Invalid hex escape sequence at line {}, treating as literal character.", lineNumber);
								sb += static_cast<char8_t>(ch);
								continue;
							}
						}
						else
						{
							escaped = 0;
							if (ch == 'n') sb += u8'\n';
							else if (ch == 'r') sb += u8'\r';
							else if (ch == 'x') ++escaped, xch = 0;
							else if (ch == '0') sb.Append(u8'\0');
							else if (ch == '\\') sb += u8'\\';
							else sb += static_cast<char8_t>(ch);
						}
					}
					else
					{
						if (ch == '\\') ++escaped;
						else sb += static_cast<char8_t>(ch);
					}
				}
			}
			catch (const std::exception& e)
			{
				logger.Error(L"Error processing escape sequences at line {}.", lineNumber);
				std::u8string utf8Line(reinterpret_cast<const char8_t*>(line.c_str()), line.length());
				logger.Note(L"Line content: {}", xybase::string::to_wstring(utf8Line));
				continue;
			}
			
			// 转换字符串格式
			std::string rawStr;
			try
			{
				rawStr = xybase::string::to_string(sb.ToString());
			}
			catch (const std::exception& e)
			{
				logger.Error(L"Failed to convert string format at line {}.", lineNumber);
				std::u8string utf8Line(reinterpret_cast<const char8_t*>(line.c_str()), line.length());
				logger.Note(L"Line content: {}", xybase::string::to_wstring(utf8Line));
				continue;
			}
			
			// 检查字符串长度
			if (rawStr.size() > size)
			{
				logger.Error(L"String size ({}) exceeded the limitation ({}) at line {}!", rawStr.size(), size, lineNumber);
				// 正确转换文件名
				std::u16string inputName = input->GetName();
				std::u16string outputName = output->GetName();
				logger.Note(L"When processing input file {}.", xybase::string::to_wstring(inputName));
				logger.Note(L"When patching output file {}.", xybase::string::to_wstring(outputName));
				logger.Note(L"Position={:X}", addr);
				continue;
			}
			
			// 写入输出流
			try
			{
				output->Seek(addr, xybase::Stream::SM_BEGIN);
				output->Write(rawStr);
			}
			catch (const std::exception& e)
			{
				logger.Error(L"Failed to write to output stream at line {}, address 0x{:X}.", lineNumber, addr);
				std::u16string inputName = input->GetName();
				std::u16string outputName = output->GetName();
				logger.Note(L"When processing input file {}.", xybase::string::to_wstring(inputName));
				logger.Note(L"When patching output file {}.", xybase::string::to_wstring(outputName));
				continue;
			}
		}
		catch (const std::exception& e)
		{
			// 将 exception::what() 的结果正确转换
			std::string errorMsg = e.what();
			std::u8string utf8ErrorMsg(reinterpret_cast<const char8_t*>(errorMsg.c_str()), errorMsg.length());
			logger.Error(L"Unexpected error processing line {}: {}", lineNumber, xybase::string::to_wstring(utf8ErrorMsg));
			std::u8string utf8Line(reinterpret_cast<const char8_t*>(line.c_str()), line.length());
			logger.Note(L"Line content: {}", xybase::string::to_wstring(utf8Line));
			continue;
		}
	}
	
	return 0;
}
