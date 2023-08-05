#pragma once

#ifndef MAPPIFIER_H__
#define MAPPIFIER_H__

#include <stack>

#include "Basic/MultiValue.h"
#include "Basic/Type.h"
#include "../Exception/InvalidOperationException.h"

namespace mule
{
	namespace Data
	{
		class Mappifier : public Basic::Type::DataHandler
		{
		public:
			virtual void OnSheetReadStart() override;
			virtual void OnSheetWriteStart() override;

			virtual void OnSheetReadEnd() override;
			virtual void OnSheetWriteEnd() override;

			virtual void OnRealmEnter(Basic::Type *realm, std::string name) override;
			virtual void OnRealmExit(Basic::Type *realm, std::string name) override;
			virtual void OnRealmEnter(Basic::Type *realm, int idx) override;
			virtual void OnRealmExit(Basic::Type *realm, int idx) override;

			virtual void OnDataRead(const Basic::MultiValue &value) override;
			virtual Basic::MultiValue OnDataWrite() override;

			virtual void AppendMetadatum(std::string name, const Basic::MultiValue &datum) override;

			Basic::MultiValue GetMap() const;
			void SetMap(mule::Data::Basic::MultiValue &value);
		protected:
			std::stack<Basic::MultiValue *> values;
			Basic::MultiValue result;
			Basic::MultiValue key;

			enum {
				DHMS_IDLE,
				DHMS_READ,
				DHMS_WRITE
			} status = DHMS_IDLE;
		};
	}
}

#endif // End of MAPPIFIER_H__
