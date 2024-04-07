#pragma once

#include "UniquePtr.hpp"

NAMESPACE_SOUP
{
	// Like std::vector<UniquePtr<T>> but more intrusive for less allocations.
	// Requires the following in T: soup::UniqueListLink<T> unique_list_link;
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

			T& operator*() const noexcept
			{
				return *node;
			}

			T* operator->() const noexcept
			{
				return node;
			}

			bool operator==(const Iterator& b) const noexcept
			{
				return node == b.node;
			}

			bool operator!=(const Iterator& b) const noexcept
			{
				return !operator==(b);
			}

			void operator++() noexcept
			{
				node = node->unique_list_link.next;
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
			for (auto node = head; node; )
			{
				auto next = node->unique_list_link.next;
				delete node;
				node = next;
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
			return emplace_back(up.release());
		}

		T* emplace_back(T* node)
		{
			// Initialise node's next-pointer to nullptr
			node->unique_list_link.next = nullptr;

			// Update prev-pointer of new node
			node->unique_list_link.prev = tail;

			// Update previous next-pointer
			if (tail)
			{
				tail->unique_list_link.next = node;
			}
			else
			{
				head = node;
			}

			// Update tail
			tail = node;

			return node;
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
				next->unique_list_link.prev = prev;
			}
			else
			{
				tail = prev;
			}

			// Update previous next-pointer
			if (prev)
			{
				prev->unique_list_link.next = next;
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
