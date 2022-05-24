#pragma once

#include <atomic>

#include "PoppedNode.hpp"

namespace soup
{
	template <typename Data>
	struct AtomicStack
	{
		struct Node
		{
			Node* next = nullptr;
			Data data;

			Node(Data&& data)
				: data(std::move(data))
			{
			}
		};

		std::atomic<Node*> head = nullptr;

		~AtomicStack() noexcept
		{
			for (Node* node = head.load(); node != nullptr; )
			{
				Node* tbd = node;
				node = node->next;
				delete tbd;
			}
		}

		Node* emplace_front(Data&& data)
		{
			Node* node = new Node(std::move(data));
			node->next = head.load();
			while (!head.compare_exchange_weak(node->next, node))
			{
			}
			return node;
		}

		PoppedNode<Node, Data> pop_front() noexcept
		{
			Node* node;
			do
			{
				node = head.load();
			} while (node && !head.compare_exchange_weak(node, node->next));
			return node;
		}
	};
}
