#include "CsvHandler.h"

#include <xystring.h>

using namespace mule::Data::Basic;

void mule::Csv::CsvOutHandler::OnSheetReadStart()
{
	if (status != CHS_IDLE)
		throw xybase::InvalidOperationException(L"This handler is not idle for read.", 10000);
	status = CHS_READ;
}

void mule::Csv::CsvOutHandler::OnSheetReadEnd()
{
	if (status != CHS_READ)
		throw xybase::InvalidOperationException(L"This handler is not reading.", 10000);
	status = CHS_IDLE;
}

void mule::Csv::CsvOutHandler::OnRealmEnter(Type *realm, const std::u16string &name)
{
	if (realm->IsComposite())
	{
		++layer;
	}
}

void mule::Csv::CsvOutHandler::OnRealmExit(Type *realm, const std::u16string &name)
{
	if (realm->IsComposite())
	{
		--layer;
		if (layer == 1 || layer == 0)
		{
			if (status == CHS_READ || status == CHS_READ_TRAILCELL)
			{
				status = CHS_READ;
				outstream->Write("\n");
			}
		}
	}
}

void mule::Csv::CsvOutHandler::OnRealmEnter(Type *realm, int idx)
{
	if (realm->IsComposite())
	{
		++layer;
	}
}

void mule::Csv::CsvOutHandler::OnRealmExit(Type *realm, int idx)
{
	if (realm->IsComposite())
	{
		--layer;
		if (layer == 1)
		{
			if (status == CHS_READ || status == CHS_READ_TRAILCELL)
			{
				status = CHS_READ;
				outstream->Write("\n");
			}
		}
	}
}

void mule::Csv::CsvOutHandler::OnDataRead(const MultiValue &value)
{
	if (status == CHS_READ)
		status = CHS_READ_TRAILCELL;
	else if (status == CHS_READ_TRAILCELL)
	{
		outstream->Write(",");
	}
	outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(value.Stringfy()).c_str()));
}
