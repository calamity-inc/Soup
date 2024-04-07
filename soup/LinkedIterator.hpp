#pragma once

#include "base.hpp"

NAMESPACE_SOUP
{
	template <typename Node, typename Data>
	struct LinkedIterator
	{
		Node* node;

		constexpr LinkedIterator(Node* node) noexcept
			: node(node)
		{
		}

		LinkedIterator& operator ++()
		{
			node = node->next;
			return *this;
		}

		void operator += (size_t num)
		{
			while (num--)
			{
				node = node->next;
			}
		}

		bool operator == (const LinkedIterator& b) const noexcept
		{
			return node == b.node;
		}

		bool operator != (const LinkedIterator& b) const noexcept
		{
			return !operator==(b);
		}

		Data& operator *()
		{
			return node->data;
		}
	};
}
