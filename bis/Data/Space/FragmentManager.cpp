#include "FragmentManager.h"

using namespace mule::Data::Space;

FragmentManager::FragmentManager()
{
}

const std::list<Fragment *> &mule::Data::Space::FragmentManager::GetFragments() const
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
		}
	}
	frags.push_back(new Fragment(frag));
}

void FragmentManager::RegisterFragment(unsigned int position, unsigned int size)
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

bool FragmentManager::IsFree(unsigned int position)
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

unsigned int FragmentManager::Alloc(size_t size, int align)
{
	size = (size + (static_cast<unsigned long long>(align) - 1)) & ~(static_cast<unsigned long long>(align) - 1);
	for (auto frag : frags)
	{
		if (frag->GetSize() >= size)
		{
			unsigned int ret = frag->GetBegining();
			frag->EliminateBegining((unsigned int)size);
			return ret;
		}
	}
	return -1;
}
