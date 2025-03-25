#include "GarbageCollector.hpp"

NAMESPACE_SOUP
{
	size_t GarbageCollector::getNumObjects() const noexcept
	{
		size_t accum = 0;
		for (GarbageCollected* node = head.load(); node; )
		{
			node = node->next.load();
			++accum;
		}
		return accum;
	}

	void GarbageCollector::add(GarbageCollected* node) noexcept
	{
		GarbageCollected* expected = head.load();
		do
		{
			node->next.store(expected);
		} while (!head.compare_exchange_weak(expected, node));
	}

	size_t GarbageCollector::tick() noexcept
	{
		size_t accum = 0;
		std::atomic<GarbageCollected*>* pNext = &head;
		for (GarbageCollected* node = pNext->load(); node; )
		{
			auto next = node->next.load();
			if (node->canBeDeleted())
			{
				GarbageCollected* expected = node;
				while (!pNext->compare_exchange_weak(expected, next))
				{
					pNext = &expected->next;
					expected = node;
				}
				++accum;
				delete node;
			}
			else
			{
				pNext = &node->next;
			}
			node = next;
		}
		return accum;
	}

	void GarbageCollector::deinit() noexcept
	{
		for (GarbageCollected* node = head.load(); node; )
		{
			auto next = node->next.load();
			delete node;
			node = next;
		}
	}
}
