#pragma once
#ifndef STRINGFIER_H__
#define STRINGFIER_H__

#include "Basic/DataHandler.h"
#include "../xybase/StringBuilder.h"

namespace mule
{
	namespace Data
	{

		class Stringfier : public Basic::DataHandler {
			Basic::MultiValue *target;
			xybase::StringBuilder<char> *sb;

		public:
			Stringfier(Basic::MultiValue *target);

			~Stringfier();

		protected:
			virtual void OnSheetReadingStarted() override;

			//virtual void OnSheetWritingStarted() override;

			virtual void OnSheetReadingEnded() override;

			//virtual void OnSheetWritingEnded() override;

			virtual void OnRecordReadingStarted() override;

			//virtual void OnRecordWritingStarted() override;

			virtual void OnRecordReadingEnded() override;

			//virtual void OnRecordWritingEnded() override;

			virtual void OnCellRead(const Basic::MultiValue &value) override;

			//virtual MultiValue OnCellWrite() override;
		};
	}
}

#endif
