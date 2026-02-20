#include "Fragment.h"

using namespace xybase::Fragment;

void Fragment::EliminateBeginning(size_t size)
{
	if (size > this->size) throw InvalidParameterException(L"size", L"Cannot larger than this->size.", __LINE__);

	begin += size;
	this->size -= size;
}

void Fragment::EliminateEnding(size_t size)
{
	if (size > this->size) throw InvalidParameterException(L"size", L"Cannot be larger than this->size.", __LINE__);

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

bool Fragment::IsContains(size_t position) const
{
	return position >= begin && position < GetEnding();
}

Fragment *Fragment::MergeToNew(const Fragment &target) const
{
	if (IsContains(target)) return new Fragment(*this);
	if (IsOverlapsWith(target))
	{
		size_t nb = begin < target.begin ? begin : target.begin;
		size_t ne = GetEnding() < target.GetEnding() ? target.GetEnding() : GetEnding();

		return new Fragment(nb, ne - nb);
	}

	return nullptr;
}

void Fragment::Merge(const Fragment &target)
{
	if (IsContains(target)) return;
	if (!IsOverlapsWith(target)) throw InvalidParameterException(L"target", L"Have no insertion with this fragment.", __LINE__);

	size_t origEnd = GetEnding();
	size_t newBegin = begin < target.begin ? begin : target.begin;
	size_t newEnd = origEnd < target.GetEnding() ? target.GetEnding() : origEnd;

	begin = newBegin;
	SetEnding(newEnd);
}

bool Fragment::operator==(const Fragment &right) const
{
	return begin == right.begin && size == right.size;
}

bool Fragment::operator!=(const Fragment &right) const
{
	return !(*this == right);
}

Fragment::Fragment(size_t begin, size_t size)
{
	this->begin = begin;
	this->size = size;
}

Fragment::Fragment(const Fragment &pattern)
{
	size = pattern.size;
	begin = pattern.begin;
}

size_t Fragment::GetBeginning() const
{
	return begin;
}

size_t Fragment::GetEnding() const
{
	return begin + size;
}

size_t Fragment::GetSize() const
{
	return size;
}

void Fragment::SetFragment(size_t beginning, size_t size)
{
	begin = beginning;
	this->size = size;
}

void Fragment::SetBeginning(size_t beginning)
{
	if (beginning >= GetEnding()) throw InvalidParameterException(L"beginning", L"Cannot be set to the pos after the ending.", __LINE__);
	size_t newSize = GetEnding() - beginning;

	begin = beginning;
	size = newSize;
}

void Fragment::SetSize(size_t size)
{
	this->size = size;
}

void Fragment::SetEnding(size_t ending)
{
	if (ending < begin) throw InvalidParameterException(L"ending", L"Cannot be set to the pos before the begining.", __LINE__);

	this->size = ending - begin;
}
