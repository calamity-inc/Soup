#include "huffman_tree.hpp"

#include "BitReader.hpp"
#include "BitWriter.hpp"

namespace soup
{
	void htNode::serialise(BitWriter& bw, void(*serialise_data_node)(BitWriter&, const htNode&)) const
	{
		reinterpret_cast<const htLinkNode*>(this)->left->serialiseImpl(bw, serialise_data_node);
		reinterpret_cast<const htLinkNode*>(this)->right->serialiseImpl(bw, serialise_data_node);
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
