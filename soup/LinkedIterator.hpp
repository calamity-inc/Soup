#pragma once

namespace soup
{
	template <typename astNode, typename Data>
	struct LinkedIterator
	{
		astNode* node;

		constexpr LinkedIterator(astNode* node) noexcept
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
