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

size_t xybase::Fragment::FragmentManager::AllocNear(size_t size, int align, size_t pos, size_t maxDistance)
{
	// 对齐请求大小
	size = (size + (static_cast<unsigned long long>(align) - 1)) & ~(static_cast<unsigned long long>(align) - 1);

	// ---------- 第一遍：尝试从已有 Fragment 的头部或尾部分配 ----------
	for (auto frag : frags)
	{
		if (frag->GetSize() >= size)
		{
			// 1) 从头部对齐分配
			size_t bgn = XY_ALIGN(frag->GetBeginning(), align);
			if (bgn >= frag->GetBeginning() && bgn + size <= frag->GetEnding())
			{
				// FIXME: 可能溢出
				if (XY_ABS(static_cast<long long>(bgn - pos)) <= maxDistance)
				{
					size_t add = bgn - frag->GetBeginning();
					frag->EliminateBeginning(size + add);
					return bgn;
				}
			}

			// 2) 从尾部对齐分配
			size_t endAligned = (frag->GetEnding() - size, align) & (~(align - 1));
			if (endAligned >= frag->GetBeginning() && endAligned + size <= frag->GetEnding())
			{
				if (XY_ABS(static_cast<long long>(endAligned - pos)) <= maxDistance)
				{
					size_t add = frag->GetEnding() - (endAligned + size);
					frag->EliminateEnding(size + add);
					return endAligned;
				}
			}
		}
	}

	// ---------- 第二遍：尝试从 Fragment 中间分配（需要分割） ----------
	for (auto it = frags.begin(); it != frags.end(); ++it)
	{
		Fragment *frag = *it;
		size_t beg = frag->GetBeginning();
		size_t end = frag->GetEnding();

		if (frag->GetSize() < size)
			continue;

		// 计算距离 pos 最近的两个对齐地址（向上对齐和向下对齐）
		size_t candidate_up = XY_ALIGN(pos, align);
		size_t candidate_down = pos & (~(static_cast<unsigned long long>(align) - 1));

		auto try_alloc = [&](size_t candidate) -> bool {
			if (candidate >= beg && candidate + size <= end &&
				XY_ABS(static_cast<long long>(candidate - pos)) <= maxDistance)
			{
				// 分割当前 Fragment
				if (candidate > beg) {
					(*it)->SetSize(candidate - beg);          // 前碎片
				}
				else {
					// 理论上不会进入，但若进入则删除原块（防止重复）
					it = frags.erase(it);
					// 注意 erase 后 it 指向下一个元素，后续插入需调整
				}
				if (candidate + size < end) {
					frags.push_back(new Fragment(candidate + size, end - (candidate + size)));
				}
				return true;
			}
			return false;
			};

		// 优先选择距离更近的候选
		long long dist_up = (candidate_up >= beg && candidate_up + size <= end) ?
			XY_ABS(static_cast<long long>(candidate_up - pos)) : maxDistance + 1;
		long long dist_down = (candidate_down >= beg && candidate_down + size <= end) ?
			XY_ABS(static_cast<long long>(candidate_down - pos)) : maxDistance + 1;

		if (dist_up <= maxDistance && dist_up <= dist_down)
		{
			if (try_alloc(candidate_up))
				return candidate_up;
		}
		else if (dist_down <= maxDistance)
		{
			if (try_alloc(candidate_down))
				return candidate_down;
		}
	}

	throw xybase::InvalidOperationException(L"Cannot alloc required block.", 32752);
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
