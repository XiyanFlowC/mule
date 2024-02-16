#pragma once

#ifndef MAPPIFIER_H__
#define MAPPIFIER_H__

#include <stack>

#include <Data/Basic/MultiValue.h>
#include <Data/Basic/Type.h>
#include <Exception/InvalidOperationException.h>

namespace mule
{
	namespace Cpp
	{
		class Mappifier : public mule::Data::Basic::Type::DataHandler, public mule::Data::Basic::Type::FileHandler
		{
		public:
			virtual void OnSheetReadStart() override;
			virtual void OnSheetWriteStart() override;

			virtual void OnSheetReadEnd() override;
			virtual void OnSheetWriteEnd() override;

			virtual void OnRealmEnter(mule::Data::Basic::Type *realm, const std::u16string &name) override;
			virtual void OnRealmExit(mule::Data::Basic::Type *realm, const std::u16string &name) override;
			virtual void OnRealmEnter(mule::Data::Basic::Type *realm, int idx) override;
			virtual void OnRealmExit(mule::Data::Basic::Type *realm, int idx) override;

			virtual void OnDataRead(const mule::Data::Basic::MultiValue &value) override;
			virtual const mule::Data::Basic::MultiValue OnDataWrite() override;

			virtual void AppendMetadatum(std::u16string name, const mule::Data::Basic::MultiValue &datum) override;

			mule::Data::Basic::MultiValue GetMap() const;
			void SetMap(mule::Data::Basic::MultiValue &value);
		protected:
			std::stack<mule::Data::Basic::MultiValue *> values;
			mule::Data::Basic::MultiValue result;
			mule::Data::Basic::MultiValue key;

			enum {
				DHMS_IDLE,
				DHMS_READ,
				DHMS_WRITE
			} status = DHMS_IDLE;
		};
	}
}

#endif // End of MAPPIFIER_H__
