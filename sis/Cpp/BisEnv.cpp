#include "BisEnv.h"

#include <Container/IsoContainer.h>
#include <Data/Array.h>
#include <Data/Referrence.h>
#include <Data/Mappifier.h>
#include <Stream/ElfStream.h>

using namespace mule::Data;

mule::Data::Basic::Type::DataHandler *BisEnvGetHandler(const char16_t *name)
{
	static char16_t mappifierName[] = u"mappifier";
	//static mule::Data::Mappifier _mapp;
	if (memcmp(name, mappifierName, sizeof(mappifierName)))
		return nullptr;
	//return &_mapp;
	return new mule::Data::Mappifier();
}

xybase::Stream *BisEnvApplyStream(const char16_t *name, xybase::Stream *infraStream)
{
	static char16_t elfName[] = u"elf";
	
	if (!memcmp(name, elfName, sizeof(elfName)))
	{
		return new mule::Stream::ElfStream(infraStream);
	}

	return nullptr;
}

xybase::FileContainer *BisEnvApplyContainer(const char16_t *name, xybase::Stream *infraStream)
{
	static char16_t isoName[] = u"iso";

	if (!memcmp(name, isoName, sizeof(isoName)))
	{
		return new mule::Container::IsoContainer(infraStream);
	}

	return nullptr;
}

TypeCreator *BisEnvGetCreators()
{
	auto ret = new Array::ArrayCreator();
	ret->nextCreator = new Referrence::ReferrenceCreator();
	ret->nextCreator->nextCreator = new BasicFieldCreator();

	return ret;
}

mule::Cpp::PluginDescription mule::Cpp::bisDesc = {
	"bis",
	"xiyan",
	"MIT",
	1, 0,
	"Basic mule features.",
	BisEnvGetCreators,
	BisEnvGetHandler,
	nullptr,
	BisEnvApplyStream,
	BisEnvApplyContainer
};
