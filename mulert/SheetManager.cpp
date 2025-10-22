#include "SheetManager.h"
#include "Mule.h"
#include "Configuration.h"
#include <xystring.h>
#include <filesystem>

using namespace mule;

void mule::SheetManager::StreamCloseHandler(xybase::Stream *stream)
{
	GetInstance().ClearSheets(stream);
}

mule::SheetManager::SheetManager()
	: streamOnProcessing(nullptr)
{
}

mule::SheetManager::~SheetManager()
{
	Shutdown();
}

void mule::SheetManager::Shutdown()
{
	for (auto &&sheets : streamSheets)
	{
		for (auto &&sheet : sheets.second)
		{
			delete sheet;
		}
	}
}

SheetManager &mule::SheetManager::GetInstance()
{
	static SheetManager _inst;
	return _inst;
}

void mule::SheetManager::WriteSheets(xybase::Stream *target, const std::u16string &handlerName)
{
	auto itr = streamSheets.find(target);
	if (itr == streamSheets.end()) return;

	auto &sheets = itr->second;
	std::u16string baseDir = Configuration::GetInstance().GetString(u"mule.sheet.basedir");
	for (auto *sheet : sheets)
	{
		logger.Info(L"Start to write sheet {}", xybase::string::to_wstring(sheet->GetName()));
		
		std::unique_ptr<mule::Data::Basic::Type::FileHandler> handler(
			Mule::GetInstance().GetFileHandler(handlerName.c_str())
		);
		if (handler == nullptr)
		{
			logger.Error(L"Get handler failed: [{}]", xybase::string::to_wstring(handlerName));
			throw xybase::InvalidParameterException(L"handlerName", L"Not a valid handler name", 64050);
		}

		auto sheetPath = xybase::string::to_string(baseDir + sheet->GetName() + u'.' + handlerName);
		auto sheetDir = sheetPath.substr(0, sheetPath.find_last_of('/'));

		std::unique_ptr<xybase::TextStream> stream;
		try {
			stream = std::make_unique<xybase::TextStream>(
				sheetPath,
				std::ios::in
			);
		}
		catch (const std::exception&)
		{
			logger.Warn(L"Failed to open sheet file: {}", xybase::string::to_wstring(sheetPath));
			continue;
		}

		try
		{
			handler->SetInStream(stream.get());

			handler->OnSheetWriteStart();
			sheet->Write(target, handler.get());
			handler->OnSheetWriteEnd();
		}
		catch (const xybase::Exception &ex)
		{
			logger.Error(L"An error occurred when writing to {}.", xybase::string::to_wstring(sheet->GetName()));
			logger.Note(L"Exception: {}", ex.GetMessage());
		}
		catch (const std::exception &ex)
		{
			logger.Error(L"Unexpected error when writing to {}.", xybase::string::to_wstring(sheet->GetName()));
			logger.Note(L"Exception: {}", xybase::string::sys_mbs_to_wcs(ex.what()));
		}
	}
}

void mule::SheetManager::ReadSheets(xybase::Stream *target, const std::u16string &handlerName)
{
	auto itr = streamSheets.find(target);
	if (itr == streamSheets.end()) return;

	auto &sheets = itr->second;
	std::u16string baseDir = Configuration::GetInstance().GetString(u"mule.sheet.basedir");
	for (auto *sheet : sheets)
	{
		logger.Info(L"Start to read sheet {}", xybase::string::to_wstring(sheet->GetName()));
		
		std::unique_ptr<mule::Data::Basic::Type::DataHandler> handler(
			Mule::GetInstance().GetDataHandler(handlerName.c_str())
		);
		if (handler == nullptr)
		{
			logger.Error(L"Get handler failed: [{}]", xybase::string::to_wstring(handlerName));
			throw xybase::InvalidParameterException(L"handlerName", L"Not a valid handler name", 64051);
		}
		
		auto sheetPath = xybase::string::to_string(baseDir + sheet->GetName() + u'.' + handlerName);
		auto sheetDir = sheetPath.substr(0, sheetPath.find_last_of('/'));
		if (!std::filesystem::exists(sheetDir))
		{
			std::filesystem::create_directories(sheetDir);
		}

		std::unique_ptr<xybase::TextStream> stream;
		try
		{
			stream = std::make_unique<xybase::TextStream>(
				sheetPath,
				std::ios::out
			);
			
			handler->SetOutStream(stream.get());

			handler->OnSheetReadStart();
			sheet->Read(target, handler.get());
			handler->OnSheetReadEnd();
		}
		catch (const xybase::Exception &ex)
		{
			logger.Error(L"An error occurred when reading from {}.", xybase::string::to_wstring(sheet->GetName()));
			logger.Note(L"Exception: {}", ex.GetMessage());
		}
		catch (const std::exception &ex)
		{
			logger.Error(L"Unexpected error when reading from {}.", xybase::string::to_wstring(sheet->GetName()));
			logger.Note(L"Exception: {}", xybase::string::sys_mbs_to_wcs(ex.what()));
		}
	}
}

void mule::SheetManager::RegisterSheet(xybase::Stream *target, mule::Data::Sheet *sheet)
{
	if (streamSheets.find(target) == streamSheets.end())
	{
		streamSheets[target] = {};
		target->OnClose += StreamCloseHandler;
	}

	auto &sheets = streamSheets[target];
	sheets.push_back(sheet);
	logger.Info(L"Sheet [{}] for stream [{}] registered.",
		xybase::string::to_wstring(sheet->GetName()),
		xybase::string::to_wstring(target->GetName()));
}

void mule::SheetManager::RemoveSheet(xybase::Stream *target, const std::u16string &name)
{
	auto itr = streamSheets.find(target);
	if (itr == streamSheets.end()) return;

	auto &sheets = itr->second;
	for (auto sheetItr = sheets.begin(); sheetItr != sheets.end();)
	{
		if ((*sheetItr)->GetName() == name)
		{
			delete *sheetItr;
			sheetItr = sheets.erase(sheetItr); // erase返回下一个有效迭代器
		}
		else
		{
			++sheetItr;
		}
	}
}

void mule::SheetManager::ClearSheets(xybase::Stream *target)
{
	auto itr = GetInstance().streamSheets.find(target);
	if (itr != GetInstance().streamSheets.end())
	{
		for (auto &&sheet : itr->second)
		{
			delete sheet;
		}
	}
	GetInstance().streamSheets.erase(target);
	target->OnClose -= StreamCloseHandler;
}
