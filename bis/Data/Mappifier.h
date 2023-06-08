#pragma once

#ifndef MAPPIFIER_H__
#define MAPPIFIER_H__

#include "Basic/DataHandler.h"

namespace mule
{
	namespace Data
	{
		/**
		 * @brief Ó³Éä»¯Æ÷
		*/
		class Mappifier : public Basic::DataHandler
		{
			Basic::MultiValue root;

			int currentIndex;

			Basic::MultiValue *currentObject;

		public:
			Mappifier();

		protected:
			virtual void OnSheetReadingStarted() override;

			virtual void OnSheetWritingStarted() override;

			virtual void OnSheetReadingEnded() override;

			virtual void OnSheetWritingEnded() override;

			virtual void OnRecordReadingStarted() override;

			virtual void OnRecordWritingStarted() override;

			virtual void OnRecordReadingEnded() override;

			virtual void OnRecordWritingEnded() override;

			virtual void OnCellRead(const Basic::MultiValue &value) override;

			virtual Basic::MultiValue OnCellWrite() override;
		};
	}
}

#endif /* MAPPIFIER_H__ */
