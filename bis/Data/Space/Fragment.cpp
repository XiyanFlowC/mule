#include "Fragment.h"

using namespace mule::Data::Space;
using namespace xybase;

void Fragment::EliminateBegining(unsigned int size)
{
	if (size > this->size) throw InvalidParameterException(u"size", u"Cannot larger than this->size.", __LINE__);

	begin += size;
	this->size -= size;
}

void Fragment::EliminateEnding(unsigned int size)
{
	if (size > this->size) throw InvalidParameterException(u"size", u"Cannot be larger than this->size.", __LINE__);

	this->size -= size;
}

bool Fragment::IsOverlapsWith(const Fragment &frag) const
{
	return !(GetEnding() < frag.begin || begin > frag.GetEnding());
}

bool Fragment::IsContains(const Fragment &frag) const
{
	return begin <= frag.begin && GetEnding() >= frag.GetEnding();
}

bool Fragment::IsContains(unsigned int position) const
{
	return position >= begin && position < GetEnding();
}

Fragment *Fragment::MergeToNew(const Fragment &target) const
{
	if (IsContains(target)) return new Fragment(*this);
	if (IsOverlapsWith(target))
	{
		unsigned int nb = begin < target.begin ? begin : target.begin;
		unsigned int ne = GetEnding() < target.GetEnding() ? target.GetEnding() : GetEnding();

		return new Fragment(nb, ne - nb);
	}

	return nullptr;
}

void Fragment::Merge(const Fragment &target)
{
	if (IsContains(target)) return;
	if (!IsOverlapsWith(target)) throw InvalidParameterException(u"target", u"Have no insertion with this fragment.", __LINE__);

	begin = begin < target.begin ? begin : target.begin;
	SetEnding(GetEnding() < target.GetEnding() ? target.GetEnding() : GetEnding());
}

bool Fragment::operator==(const Fragment &right) const
{
	return begin == right.begin && size == right.size;
}

bool Fragment::operator!=(const Fragment &right) const
{
	return !(*this == right);
}

Fragment::Fragment(unsigned int begin, unsigned size)
{
	this->begin = begin;
	this->size = size;
}

Fragment::Fragment(const Fragment &pattern)
{
	size = pattern.size;
	begin = pattern.begin;
}

unsigned int Fragment::GetBegining() const
{
	return begin;
}

unsigned int Fragment::GetEnding() const
{
	return begin + size;
}

unsigned int Fragment::GetSize() const
{
	return size;
}

void Fragment::SetFragment(unsigned int begining, unsigned int size)
{
	begin = begining;
	this->size = size;
}

void Fragment::SetBegining(unsigned int begining)
{
	if (begining >= GetEnding()) throw InvalidParameterException(u"begining", u"Cannot be set to the pos after the ending.", __LINE__);
	int newSize = GetEnding() - begining;

	begin = begining;
	size = newSize;
}

void Fragment::SetSize(unsigned int size)
{
	this->size = size;
}

void Fragment::SetEnding(unsigned int ending)
{
	if (ending < begin) throw InvalidParameterException(u"ending", u"Cannot be set to the pos before the begining.", __LINE__);

	this->size = ending - begin;
}
