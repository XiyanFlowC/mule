#pragma once

#ifndef DATA_HANDLER_H__
#define DATA_HANDLER_H__

#include <string>
#include "MultiValue.h"
#include "../../xybase/Stream.h"
#include "../../xybase/StringBuilder.h"
#include "../../Exception/InvalidOperationException.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{

			class DataHandler
			{
				enum
				{
					DHS_IDLE,
					DHS_SHEET_READ,
					DHS_RECORD_READ,
					DHS_SHEET_WRITE,
					DHS_RECORD_WRITE,
				} status;

			public:
				void BeforeSheetRead(std::string tableName);

				void BeforeSheetWrite(std::string tableName);

				void AfterSheetRead();

				void AfterSheetWrite();

				void BeforeRecordRead();

				void BeforeRecordWrite();

				void AfterRecordRead();

				void AfterRecordWrite();

				void AfterCellRead(const MultiValue &value);

				const MultiValue BeforeCellWrite();

				DataHandler();
			protected:
				std::string tableName;

				virtual void OnSheetReadingStarted();

				virtual void OnSheetWritingStarted();

				virtual void OnSheetReadingEnded();

				virtual void OnSheetWritingEnded();

				virtual void OnRecordReadingStarted();

				virtual void OnRecordWritingStarted();

				virtual void OnRecordReadingEnded();

				virtual void OnRecordWritingEnded();

				virtual void OnCellRead(const MultiValue &value);

				virtual MultiValue OnCellWrite();
			};
		}
	}
}

#endif
