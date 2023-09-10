#include "CStyleInitHandler.h"

#include <xystring.h>
#include <Exception/NotImplementedException.h>

using namespace mule::Data::Basic;

int mule::Cpp::CStyleInitHandler::ident = 1, mule::Cpp::CStyleInitHandler::type = 0;

mule::Cpp::CStyleInitHandler::CStyleInitHandler()
	: stream {nullptr}, state{ CSIHS_IDLE }
{
}

void mule::Cpp::CStyleInitHandler::OnRealmEnter(Type *realm, const std::u16string &name)
{
	if (realm->IsComposite())
	{
		for (int i = 0; i < layer; ++i)
			for (int j = 0; j < ident; ++j)
			{
				stream->Write(type ? " " : "\t");
			}
		++layer;
		stream->Write("{\n");
	}
}

void mule::Cpp::CStyleInitHandler::OnRealmExit(Type *realm, const std::u16string &name)
{
	if (realm->IsComposite())
	{
		--layer;
		for (int i = 0; i < layer; ++i)
			for (int j = 0; j < ident; ++j)
			{
				stream->Write(type ? " " : "\t");
			}
		stream->Write("},\n");
	}
}

void mule::Cpp::CStyleInitHandler::OnRealmEnter(Type *realm, int idx)
{
	if (realm->IsComposite())
	{
		for (int i = 0; i < layer; ++i)
			for (int j = 0; j < ident; ++j)
			{
				stream->Write(type ? " " : "\t");
			}
		++layer;
		stream->Write("{\n");
	}
}

void mule::Cpp::CStyleInitHandler::OnRealmExit(Type *realm, int idx)
{
	if (realm->IsComposite())
	{
		--layer;
		for (int i = 0; i < layer; ++i)
			for (int j = 0; j < ident; ++j)
			{
				stream->Write(type ? " " : "\t");
			}
		stream->Write("},\n");
	}
}

void mule::Cpp::CStyleInitHandler::OnDataRead(const MultiValue &value)
{
	for (int i = 0; i < layer; ++i)
		for (int j = 0; j < ident; ++j)
		{
			stream->Write(type ? " " : "\t");
		}
	stream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(value.Stringfy()).c_str()));
	stream->Write(",\n");
}

MultiValue mule::Cpp::CStyleInitHandler::OnDataWrite()
{
	return MultiValue();
}

void mule::Cpp::CStyleInitHandler::SetStream(xybase::Stream *stream)
{
	this->stream = stream;
}

void mule::Cpp::CStyleInitHandler::OnSheetReadStart()
{
	if (state != CSIHS_IDLE)
		throw xybase::InvalidOperationException(L"This handler is not idle for read.", 10000);
	state = CSIHS_READ;
}

void mule::Cpp::CStyleInitHandler::OnSheetReadEnd()
{
	if (state != CSIHS_READ)
		throw xybase::InvalidOperationException(L"This handler is not reading.", 10000);
	state = CSIHS_IDLE;
}

void mule::Cpp::CStyleInitHandler::OnSheetWriteStart()
{
	throw xybase::NotImplementedException();
	if (state != CSIHS_IDLE)
		throw xybase::InvalidOperationException(L"This handler is not idle for write.", 10000);
	state = CSIHS_WRITE;
}

void mule::Cpp::CStyleInitHandler::OnSheetWriteEnd()
{
	if (state != CSIHS_WRITE)
		throw xybase::InvalidOperationException(L"This handler is not writing.", 10000);
	state = CSIHS_IDLE;
}
