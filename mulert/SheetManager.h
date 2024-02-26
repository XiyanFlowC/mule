#pragma once

#include "Data/Sheet.h"
#include "mulert_api.h"
#include "Logger.h"
#include <map>
#include <list>

namespace mule
{
	class MULERT_API SheetManager
	{
		std::map<xybase::Stream *, std::list<mule::Data::Sheet *>> streamSheets;

		xybase::Stream *streamOnProcessing;
		std::list<mule::Data::Sheet *> suspendedSheets;

		static void StreamCloseHandler(xybase::Stream *stream);

		Logger logger = Logger::GetLogger<SheetManager>();

	public:
		SheetManager();

		static SheetManager &GetInstance();

		void WriteSheets(xybase::Stream *target, const std::u16string &handlerName);

		void ReadSheets(xybase::Stream *target, const std::u16string &handlerName);

		/**
		 * @brief Register a sheet to a stream. If the stream has not been registered yet, register it first.
		 * @param target Target stream.
		 * @param sheet The sheet needs to append.
		*/
		void RegisterSheet(xybase::Stream *target, mule::Data::Sheet *sheet);

		void RemoveSheet(xybase::Stream *target, const std::u16string &name);

		void ClearSheets(xybase::Stream *target);
	};
}
