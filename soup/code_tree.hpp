#pragma once

#include "TreeNode.hpp"

#include <vector>

#define SOUP_CODE_TREE_NODE(type, ...) if (::soup::CodeTreeNodeRaii<type> _{}; new type(__VA_ARGS__), true)

namespace soup
{
	template <typename T>
	struct CodeTreeNode : public TreeNode
	{
		inline static thread_local T* root = nullptr;
		inline static thread_local T* current = nullptr;

		T* const parent;
		std::vector<T*> children{};

		CodeTreeNode() noexcept
			: parent(current)
		{
			if (parent == nullptr)
			{
				root = reinterpret_cast<T*>(this);
			}
			else
			{
				parent->children.emplace_back(reinterpret_cast<T*>(this));
			}
			current = reinterpret_cast<T*>(this);
		}

		static void finishCurrentNode()
		{
			current = current->parent;

			if (current == nullptr)
			{
				root->onFinishedBuildingTree();
			}
		}

		// Will be called on the root node and is expected to erase the tree (unless you want to leak it)
		void onFinishedBuildingTree()
		{
			erase();
		}

		void erase() noexcept
		{
			for (auto& child : children)
			{
				child->erase();
			}
			delete this;
		}
	};

	template <typename T>
	struct CodeTreeNodeRaii
	{
		~CodeTreeNodeRaii() noexcept
		{
			T::finishCurrentNode();
		}
	};
}
