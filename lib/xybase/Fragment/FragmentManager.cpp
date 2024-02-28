#include "FragmentManager.h"

#include "../xyutils.h"
#include "../Exception/InvalidOperationException.h"

using namespace xybase::Fragment;

FragmentManager::~FragmentManager()
{
	for (auto ptr : frags)
	{
		delete ptr;
	}
}

FragmentManager::FragmentManager()
{
}

const std::list<Fragment *> &FragmentManager::GetFragments() const
{
	return frags;
}

void FragmentManager::RegisterFragment(const Fragment &frag)
{
	for (auto f : frags)
	{
		if (f->IsContains(frag))
		{
			return;
		}
		if (f->IsOverlapsWith(frag))
		{
			f->Merge(frag);
			return;
		}
	}
	frags.push_back(new Fragment(frag));
}

void FragmentManager::RegisterFragment(size_t position, size_t size)
{
	RegisterFragment(Fragment(position, size));
}

void FragmentManager::Defragment()
{
	for (auto itr = frags.begin(); itr != frags.end(); ++itr)
	{
		std::list<Fragment *>::iterator jtr = itr;
		++jtr;
		while (jtr != frags.end())
		{
			if ((*itr)->IsContains(**jtr))
			{
				delete *jtr;
				frags.erase(jtr++);
			}
			else if ((*itr)->IsOverlapsWith(**jtr))
			{
				(**itr).Merge(**jtr);
				delete *jtr;
				frags.erase(jtr++);
			}
			else jtr++;
		}
	}
}

bool FragmentManager::IsFree(size_t position)
{
	for (auto frag : frags)
	{
		if (frag->IsContains(position)) return false;
	}
	return true;
}

bool FragmentManager::IsFree(Fragment *frag)
{
	for (auto frag : frags)
	{
		if (frag->IsContains(*frag)) return false;
	}
	return true;
}

size_t FragmentManager::Alloc(size_t size, int align)
{
	size = (size + (static_cast<unsigned long long>(align) - 1)) & ~(static_cast<unsigned long long>(align) - 1);
	// 尝试分配已经对齐的空间
	for (auto frag : frags)
	{
		if (frag->GetSize() >= size && !(frag->GetBeginning() & (align - 1)))
		{
			size_t ret = frag->GetBeginning();
			frag->EliminateBeginning((size_t)size);
			return ret;
		}
	}
	// 若都不对齐
	for (auto frag : frags)
	{
		// 若可能具有空位
		if (frag->GetSize() >= size)
		{
			// 计算对齐后的大小
			auto bgn = frag->GetBeginning();
			bgn = XY_ALIGN(bgn, align);
			size_t add = bgn - frag->GetBeginning();
			if (frag->GetSize() >= size + add)
			{
				// 返回对齐的空间
				frag->EliminateBeginning((size_t)size + add);
				return bgn;
			}
		}
	}
	throw xybase::InvalidOperationException(L"Cannot alloc anymore.", 32751);
}

Fragment xybase::Fragment::FragmentManager::AllocMaximumFragment()
{
	size_t max = 0;
	for (auto &&frag : frags)
	{
		if (frag->GetSize() > max)
		{
			max = frag->GetSize();
		}
	}
	return Fragment(Alloc(max, 1), max);
}
