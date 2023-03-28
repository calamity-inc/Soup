#pragma once

#include "UniquePtr.hpp"

namespace soup
{
	// Like std::vector<UniquePtr<T>> but more intrusive for less allocations.
	// Requires the following in T: struct { T* next; T* prev; } unique_list_link;
	template <typename T>
	struct UniqueList
	{
		struct Iterator
		{
			T* node;

			Iterator(T* node)
				: node(node)
			{
			}

			void operator++(int) noexcept
			{
				return node->next;
			}

			T* operator->() const noexcept
			{
				return node;
			}
		};

		T* head = nullptr;
		T* tail = nullptr;

		UniqueList() = default;
		UniqueList(const UniqueList<T>&) = delete;

		UniqueList(UniqueList<T>&& b)
			: head(b.head), tail(b.tail)
		{
			b.head = nullptr;
			b.tail = nullptr;
		}

		~UniqueList()
		{
			for (auto node = head; node; node = node->unique_list_link.next)
			{
				delete node;
			}
		}

		[[nodiscard]] Iterator begin() const noexcept
		{
			return head;
		}

		[[nodiscard]] Iterator end() const noexcept
		{
			return nullptr;
		}

		[[nodiscard]] size_t size() const noexcept
		{
			size_t s = 0;
			for (auto node = head; node; node = node->unique_list_link.next)
			{
				++s;
			}
			return s;
		}

		T* emplace_back(UniquePtr<T>&& up)
		{
			T* node = up.release();

			// Update prev-pointer of new node
			node->unique_list_link.prev = tail;

			// Update previous next-pointer
			if (tail)
			{
				tail->next = node;
			}
			else
			{
				head = node;
			}

			// Update tail
			tail = node;
		}

		Iterator erase(Iterator it)
		{
			T* node = it.node;
			Iterator ret = node->unique_list_link.next;
			erase(node);
			return ret;
		}

		void erase(T* node)
		{
			T* next = node->unique_list_link.next;
			T* prev = node->unique_list_link.prev;

			// Update next prev-pointer
			if (next)
			{
				next->prev = prev;
			}
			else
			{
				tail = prev;
			}

			// Update previous next-pointer
			if (prev)
			{
				prev->next = next;
			}
			else
			{
				head = next;
			}

			// Yeetus deletus
			delete node;
		}
	};
}
