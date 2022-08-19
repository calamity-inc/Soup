#pragma once

#include "fwd.hpp"

#include <algorithm>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "TreeNode.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	template <typename Data>
	struct htDataNode;

	class htNode : public TreeNode
	{
	public:
		const bool is_data;

		htNode(bool is_data)
			: is_data(is_data)
		{
		}

		template <typename Data, typename Weight>
		[[nodiscard]] Weight getWeight() const noexcept;

		void serialise(BitWriter& bw, void(*serialise_data_node)(BitWriter&, const htNode&)) const;
	private:
		void serialiseImpl(BitWriter& bw, void(*serialise_data_node)(BitWriter&, const htNode&)) const;

	public:
		[[nodiscard]] std::optional<std::vector<bool>> find(const void* ud, bool(*compare_data_node)(const htNode&, const void*), std::vector<bool>&& path = {}) const;

		void encode(BitWriter& bw, const void* ud, bool(*compare_data_node)(const htNode&, const void*)) const;
		static void encode(BitWriter& bw, const std::vector<bool>& path);

		template <typename Data>
		[[nodiscard]] const htDataNode<Data>& asDataNode() const noexcept
		{
			return *reinterpret_cast<const htDataNode<Data>*>(this);
		}

		template <typename Data>
		[[nodiscard]] const Data& getData() const noexcept
		{
			return asDataNode<Data>().data;
		}
	};

	struct htLinkNode : public htNode
	{
		UniquePtr<htNode> left;
		UniquePtr<htNode> right;

		htLinkNode()
			: htNode(false)
		{
		}
	};

	template <typename Data>
	struct htDataNode : public htNode
	{
		Data data;

		htDataNode(Data&& data)
			: htNode(true), data(std::move(data))
		{
		}
	};

	template <typename Data, typename Weight>
	struct htWeightedDataNode : public htDataNode<Data>
	{
		Weight weight;

		htWeightedDataNode(std::pair<Data, Weight>&& p)
			: htDataNode<Data>(std::move(p.first)), weight(std::move(p.second))
		{
		}
	};

	template<typename Data, typename Weight>
	inline Weight htNode::getWeight() const noexcept
	{
		if (is_data)
		{
			return reinterpret_cast<const htWeightedDataNode<Data, Weight>&>(*this).weight;
		}
		return reinterpret_cast<const htLinkNode*>(this)->left->getWeight<Data, Weight>()
			+ reinterpret_cast<const htLinkNode*>(this)->right->getWeight<Data, Weight>();
	}

	template <typename Data, typename Weight>
	[[nodiscard]] inline UniquePtr<htNode> htGenerate(std::vector<std::pair<Data, Weight>>&& data)
	{
		using Pair = std::pair<Data, Weight>;

		if (data.empty())
		{
			return {};
		}

		std::sort(data.begin(), data.end(), [](const Pair& a, const Pair& b)
		{
			return a.second > b.second;
		});

		std::vector<UniquePtr<htNode>> flat{};
		for (Pair& p : data)
		{
			flat.emplace_back(soup::make_unique<htWeightedDataNode<Data, Weight>>(std::move(p)));
		}

		while (flat.size() != 1)
		{
			typename std::vector<UniquePtr<htNode>>::iterator l = flat.begin();
			typename std::vector<UniquePtr<htNode>>::iterator r = l + 1;

			auto l_w = (*l)->getWeight<Data, Weight>();
			auto r_w = (*r)->getWeight<Data, Weight>();

			if (flat.size() > 2)
			{
				for (auto i = l + 2; i != flat.end(); ++i)
				{
					auto w = (*i)->getWeight<Data, Weight>();
					if (w < l_w)
					{
						l = i;
						l_w = w;
						if (l_w < r_w)
						{
							std::swap(l, r);
							std::swap(l_w, r_w);
						}
					}
				}
			}

			auto link = soup::make_unique<htLinkNode>();
			link->left = std::move(*l);
			link->right = std::move(*r);

			size_t li = std::distance(flat.begin(), l);
			size_t ri = std::distance(flat.begin(), r);
			flat.erase(l);
			if (li <= ri)
			{
				--ri;
			}
			flat.erase(flat.begin() + ri);

			flat.emplace_back(std::move(link));
		}

		UniquePtr<htNode> root = std::move(flat.at(0));
		return root;
	}

	template <typename Data, typename Weight = void>
	inline void htPrint(const htNode& node, const std::string& prefix = {})
	{
		std::cout << prefix;
		if constexpr (!std::is_same_v<Weight, void>)
		{
			std::cout << '[';
			std::cout << node.getWeight<Data, Weight>();
			std::cout << "] ";
		}
		if (node.is_data)
		{
			std::cout << reinterpret_cast<const htDataNode<Data>&>(node).data << "\n";
		}
		else
		{
			std::cout << "<link>\n";

			std::string p2 = prefix;
			p2.append("\t");

			htPrint<Data, Weight>(*reinterpret_cast<const htLinkNode&>(node).left, p2);
			htPrint<Data, Weight>(*reinterpret_cast<const htLinkNode&>(node).right, p2);
		}
	}

	[[nodiscard]] extern UniquePtr<htNode> htDeserialise(BitReader& br, htNode* (*deserialise_data_node)(BitReader&)); // deserialise_data_node should use `new htDataNode`, don't worry, it won't leak.
}
