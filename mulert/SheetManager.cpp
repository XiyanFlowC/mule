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
		mule::Data::Basic::Type::FileHandler *handler = Mule::GetInstance().GetFileHandler(handlerName.c_str());
		if (handler == nullptr)
		{
			logger.Error(L"Get handler failed: [{}]", xybase::string::to_wstring(handlerName));
			throw xybase::InvalidParameterException(L"handlerName", L"Not a valid handler name", 64050);
		}

		auto sheetPath = xybase::string::to_string(baseDir + sheet->GetName() + u'.' + handlerName);
		auto sheetDir = sheetPath.substr(0, sheetPath.find_last_of('/'));

		xybase::TextStream *stream = nullptr;
		try {
			stream = new xybase::TextStream(
				sheetPath,
				std::ios::in
			);
		}
		catch (xybase::Exception &ex)
		{
			delete handler;
			continue;
		}

		try
		{
			handler->SetInStream(stream);

			handler->OnSheetWriteStart();
			sheet->Write(target, handler);
			handler->OnSheetWriteEnd();
		}
		catch (xybase::Exception &ex)
		{
			logger.Error(L"An error occurred when writing to {}.", xybase::string::to_wstring(sheet->GetName()));
			logger.Note(L"Exception: {}", ex.GetMessage());
		}

		delete handler;
		delete stream;
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
		mule::Data::Basic::Type::DataHandler *handler = Mule::GetInstance().GetDataHandler(handlerName.c_str());
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

		xybase::TextStream *stream = new xybase::TextStream(
			sheetPath,
			std::ios::out
		);

		try
		{
			handler->SetOutStream(stream);

			handler->OnSheetReadStart();
			sheet->Read(target, handler);
			handler->OnSheetReadEnd();
		}
		catch (xybase::Exception &ex)
		{
			logger.Error(L"An error occurred when reading from {}.", xybase::string::to_wstring(sheet->GetName()));
			logger.Note(L"Exception: {}", ex.GetMessage());
		}

		delete handler;
		delete stream;
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
	auto sheetItr = sheets.begin();
	if (sheetItr == sheets.end()) return;

	while (sheetItr != sheets.end())
	{
		if ((*sheetItr)->GetName() == name)
		{
			sheets.erase(sheetItr);
			delete *sheetItr;
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
