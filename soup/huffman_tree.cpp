#include "huffman_tree.hpp"

#include "BitReader.hpp"
#include "BitWriter.hpp"

NAMESPACE_SOUP
{
	void htNode::serialise(BitWriter& bw, void(*serialise_data_node)(BitWriter&, const htNode&)) const
	{
		static_cast<const htLinkNode*>(this)->left->serialiseImpl(bw, serialise_data_node);
		static_cast<const htLinkNode*>(this)->right->serialiseImpl(bw, serialise_data_node);
	}

	void htNode::serialiseImpl(BitWriter& bw, void(*serialise_data_node)(BitWriter&, const htNode&)) const
	{
		bw.b(is_data);
		if (is_data)
		{
			serialise_data_node(bw, *this);
		}
		else
		{
			serialise(bw, serialise_data_node);
		}
	}

	std::optional<std::vector<bool>> htNode::find(const void* ud, bool(*compare_data_node)(const htNode&, const void*), std::vector<bool>&& path) const
	{
		if (is_data)
		{
			if (compare_data_node(*this, ud))
			{
				return { std::move(path) };
			}
			return std::nullopt;
		}

		std::vector<bool> lp = path;
		lp.emplace_back(true);
		if (auto res = static_cast<const htLinkNode*>(this)->left->find(ud, compare_data_node, std::move(lp)); res.has_value())
		{
			return res;
		}

		std::vector<bool> rp = std::move(path);
		rp.emplace_back(false);
		return static_cast<const htLinkNode*>(this)->right->find(ud, compare_data_node, std::move(rp));
	}

	void htNode::encode(BitWriter& bw, const void* ud, bool(*compare_data_node)(const htNode&, const void*)) const
	{
		encode(bw, find(ud, compare_data_node).value());
	}

	void htNode::encode(BitWriter& bw, const std::vector<bool>& path)
	{
		for (const bool& b : path)
		{
			bw.b(b);
		}
	}

	[[nodiscard]] static UniquePtr<htNode> htDeserialiseImpl(BitReader& br, htNode* (*deserialise_data_node)(BitReader&));

	UniquePtr<htNode> htDeserialise(BitReader& br, htNode*(*deserialise_data_node)(BitReader&))
	{
		auto link = soup::make_unique<htLinkNode>();
		link->left = htDeserialiseImpl(br, deserialise_data_node);
		link->right = htDeserialiseImpl(br, deserialise_data_node);
		return link;
	}

	[[nodiscard]] static UniquePtr<htNode> htDeserialiseImpl(BitReader& br, htNode*(*deserialise_data_node)(BitReader&))
	{
		bool is_data;
		br.b(is_data);
		if (is_data)
		{
			return deserialise_data_node(br);
		}
		return htDeserialise(br, deserialise_data_node);
	}
}
