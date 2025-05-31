#include "json.hpp"

#include "filesystem.hpp"
#include "JsonArray.hpp"
#include "JsonBool.hpp"
#include "JsonFloat.hpp"
#include "JsonInt.hpp"
#include "JsonNull.hpp"
#include "JsonObject.hpp"
#include "JsonString.hpp"
#include "Reader.hpp"
#include "string.hpp"

NAMESPACE_SOUP
{
	UniquePtr<JsonNode> json::decode(const char* data, size_t size, int max_depth)
	{
		JsonTreeWriter jtw;
		jtw.allocArray = [](void*) -> void* { return new JsonArray(); };
		jtw.allocObject = [](void*) -> void* { return new JsonObject(); };
		jtw.allocString = [](void*, std::string&& value) -> void* { return new JsonString(std::move(value)); };
		jtw.allocInt = [](void*, int64_t value) -> void* { return new JsonInt(value); };
		jtw.allocFloat = [](void*, double value) -> void* { return new JsonFloat(value); };
		jtw.allocBool = [](void*, bool value) -> void* { return new JsonBool(value); };
		jtw.allocNull = [](void*) -> void* { return new JsonNull(); };
		jtw.addToArray = [](void*, void* arr, void* value) -> void { ((JsonArray*)arr)->children.emplace_back((JsonNode*)value); };
		jtw.addToObject = [](void*, void* obj, void* key, void* value) -> void { ((JsonObject*)obj)->children.emplace_back((JsonNode*)key, (JsonNode*)value); };
		jtw.free = [](void*, void* node) -> void { delete (JsonNode*)node; };
		return (JsonNode*)decode(jtw, nullptr, data, size, max_depth);
	}

	UniquePtr<JsonNode> json::decodeFile(const std::filesystem::path& path, int max_depth)
	{
		UniquePtr<JsonNode> res;
		size_t size;
		if (auto data = filesystem::createFileMapping(path, size))
		{
			res = json::decode((const char*)data, size, max_depth);
			filesystem::destroyFileMapping(data, size);
		}
		return res;
	}

	void* json::decode(const JsonTreeWriter& tw, void* user_data, const char*& c, size_t& s, int max_depth)
	{
		SOUP_ASSERT(max_depth-- != 0, "Depth limit exceeded");

		handleLeadingSpace(c, s);

		switch (s != 0 ? *c : 0)
		{
		case '"': {
			++c; --s;
			const auto encoded_size = JsonString::getEncodedSize(c, s);
			if (tw.allocUnescapedString && std::string_view(c, encoded_size).find('\\') == std::string::npos)
			{
				const auto str = tw.allocUnescapedString(user_data, c, encoded_size);
				c += encoded_size;
				s -= encoded_size;
				SOUP_IF_LIKELY (s != 0)
				{
					++c; --s;
				}
				return str;
			}
			else
			{
				std::string value;
				value.reserve(encoded_size);
				JsonString::decodeValue(value, c, s);
				value.shrink_to_fit();
				return tw.allocString(user_data, std::move(value));
			}
		}

		case '[': {
			++c; --s;
			auto arr = tw.allocArray(user_data);
			while (true)
			{
				handleLeadingSpace(c, s);
				auto val = decode(tw, user_data, c, s, max_depth);
				SOUP_IF_UNLIKELY (!val)
				{
					break;
				}
				tw.addToArray(user_data, arr, val);
				while (s != 0 && (*c == ',' || string::isSpace(*c)))
				{
					++c; --s;
				}
				if (s == 0 || *c == ']')
				{
					break;
				}
			}
			if (tw.onArrayFinished)
			{
				tw.onArrayFinished(user_data, arr);
			}
			SOUP_IF_LIKELY (s != 0)
			{
				++c; --s;
			}
			return arr;
		}

		case '{': {
			++c; --s;
			auto obj = tw.allocObject(user_data);
			while (true)
			{
				handleLeadingSpace(c, s);
				if (s == 0 || *c == '}')
				{
					break;
				}
				auto key = decode(tw, user_data, c, s, max_depth);
				while (s != 0 && (string::isSpace(*c) || *c == ':'))
				{
					++c; --s;
				}
				auto val = decode(tw, user_data, c, s, max_depth);
				SOUP_IF_UNLIKELY (!key || !val)
				{
					if (val)
					{
						tw.free(user_data, val);
					}
					if (key)
					{
						tw.free(user_data, key);
					}
					break;
				}
				tw.addToObject(user_data, obj, key, val);
				while (s != 0 && (*c == ',' || string::isSpace(*c)))
				{
					++c; --s;
				}
			}
			if (tw.onObjectFinished)
			{
				tw.onObjectFinished(user_data, obj);
			}
			SOUP_IF_LIKELY (s != 0)
			{
				++c; --s;
			}
			return obj;
		}
		}

		std::string buf{};
		bool is_int = true;
		bool is_float = false;
		for (; s != 0 && *c != ',' && !string::isSpace(*c) && *c != '}' && *c != ']' && *c != ':'; ++c, --s)
		{
			if ((is_int || is_float) && (*c == 'e' || *c == 'E'))
			{
				break;
			}

			buf.push_back(*c);

			if (!string::isNumberChar(*c) && *c != '-')
			{
				is_int = false;
				is_float = (*c == '.');
			}
		}
		int exponent = 0;
		if (*c == 'e' || *c == 'E')
		{
			++c; --s;
			is_int = false;
			is_float = true;

			const bool negative = (*c == '-');
			if (!negative && *c != '+')
			{
				return {};
			}
			++c; --s;

			for (; s != 0 && *c != ',' && !string::isSpace(*c) && *c != '}' && *c != ']' && *c != ':'; ++c, --s)
			{
				exponent *= 10;
				exponent += ((*c) - '0');
			}
			if (negative)
			{
				exponent *= -1;
			}
		}
		if (!buf.empty())
		{
			if (is_int)
			{
				auto opt = string::toIntOpt<int64_t>(buf);
				if (opt.has_value())
				{
					return tw.allocInt(user_data, opt.value());
				}
			}
			else if (is_float)
			{
				char* str_end;
				auto val = std::strtod(buf.c_str(), &str_end);
				if (str_end != buf.c_str() && val != HUGE_VAL)
				{
					if (exponent != 0)
					{
						val *= std::pow(10.0, exponent);
					}
					return tw.allocFloat(user_data, val);
				}
			}
			else if (buf == "true")
			{
				return tw.allocBool(user_data, true);
			}
			else if (buf == "false")
			{
				return tw.allocBool(user_data, false);
			}
			else if (buf == "null")
			{
				return tw.allocNull(user_data);
			}
		}
		return nullptr;
	}

	UniquePtr<JsonNode> json::binaryDecode(Reader& r)
	{
		uint8_t b;
		SOUP_IF_LIKELY (r.u8(b))
		{
			uint8_t type = (b & 0b111);
			if (type == JSON_INT)
			{
				uint8_t extra = (b >> 3);
				int64_t val;
				SOUP_IF_LIKELY (extra == 0b11111
					? r.i64_dyn(val)
					: (val = extra, true)
					)
				{
					return soup::make_unique<JsonInt>(val);
				}
			}
			else if (type == JSON_FLOAT)
			{
				uint64_t val;
				SOUP_IF_LIKELY (r.u64_le(val))
				{
					return soup::make_unique<JsonFloat>(*reinterpret_cast<double*>(&val));
				}
			}
			else if (type == JSON_STRING)
			{
				uint8_t len = (b >> 3);
				std::string val;
				SOUP_IF_LIKELY (len == 0b11111
					? r.str_lp_u64_dyn(val)
					: r.str(len, val)
					)
				{
					return soup::make_unique<JsonString>(std::move(val));
				}
			}
			else if (type == JSON_BOOL)
			{
				return soup::make_unique<JsonBool>(b >> 3);
			}
			else if (type == JSON_NULL)
			{
				return soup::make_unique<JsonNull>();
			}
			else if (type == JSON_ARRAY)
			{
				auto arr = soup::make_unique<JsonArray>();
				while (true)
				{
					UniquePtr<JsonNode> node;

					SOUP_IF_UNLIKELY (node = binaryDecode(r), !node)
					{
						break;
					}

					arr->children.emplace_back(std::move(node));
				}
				return arr;
			}
			else if (type == JSON_OBJECT)
			{
				auto obj = soup::make_unique<JsonObject>();
				while (true)
				{
					UniquePtr<JsonNode> key;
					UniquePtr<JsonNode> val;

					SOUP_IF_UNLIKELY (key = binaryDecode(r), !key)
					{
						break;
					}
					SOUP_IF_UNLIKELY (val = binaryDecode(r), !val)
					{
						break;
					}

					obj->children.emplace_back(std::move(key), std::move(val));
				}
				return obj;
			}
		}
		return {};
	}

	UniquePtr<JsonNode> json::binaryDecodeV2(Reader& r)
	{
		uint8_t b;
		SOUP_IF_LIKELY (r.u8(b))
		{
			switch (b & 0b11)
			{
			case 0:
				switch (b >> 2)
				{
				case 0: // False
					return soup::make_unique<JsonBool>(false);

				case 1: // True
					return soup::make_unique<JsonBool>(true);

				case 2: // Null
					return soup::make_unique<JsonNull>();

				case 3: // Float
					{
						uint64_t val;
						SOUP_IF_LIKELY (r.u64_le(val))
						{
							return soup::make_unique<JsonFloat>(*reinterpret_cast<double*>(&val));
						}
					}
					break;

				case 4: // Array
					{
						auto arr = soup::make_unique<JsonArray>();
						while (true)
						{
							UniquePtr<JsonNode> node;

							SOUP_IF_UNLIKELY (node = binaryDecodeV2(r), !node)
							{
								break;
							}

							arr->children.emplace_back(std::move(node));
						}
						return arr;
					}
					break;

				case 5: // Object
					{
						auto obj = soup::make_unique<JsonObject>();
						while (true)
						{
							UniquePtr<JsonNode> key;
							UniquePtr<JsonNode> val;

							SOUP_IF_UNLIKELY (key = binaryDecodeV2(r), !key)
							{
								break;
							}
							SOUP_IF_UNLIKELY (val = binaryDecodeV2(r), !val)
							{
								break;
							}

							obj->children.emplace_back(std::move(key), std::move(val));
						}
						return obj;
					}
					break;

				case 0b111111: // End of array/object (0xfc)
					break;
				}
				break;

			case 1: // String
			{
				size_t size = (b >> 2) & 0b11111;
				bool bigger = (b >> 7) & 1;
				if (bigger)
				{
					uint64_t extra;
					SOUP_RETHROW_FALSE(r.u64_dyn_v2(extra));
					size |= (extra << 5);
				}
				std::string value;
				SOUP_IF_UNLIKELY (!r.str(size, value))
				{
					return {};
				}
				return soup::make_unique<JsonString>(std::move(value));
			}

			case 2: // Positive Int (0b10)
			case 3: // Negative Int (0b11)
			{
				bool neg = b & 1;
				uint64_t u = (b >> 2) & 0b11111;
				bool more = (b >> 7) & 1;
				if (more)
				{
					uint64_t extra;
					SOUP_RETHROW_FALSE(r.u64_dyn_v2(extra));
					u |= (extra << 5);
				}

				int64_t value;
				if (neg)
				{
					value = (u * -1) - 1;
				}
				else
				{
					value = u;
				}

				return soup::make_unique<JsonInt>(value);
			}
			}
		}
		return {};
	}

	void json::handleLeadingSpace(const char*& c, size_t& s)
	{
		while (s != 0)
		{
			if (string::isSpace(*c))
			{
				++c; --s;
			}
			else if (*c == '/')
			{
				handleComment(c, s);
			}
			else
			{
				break;
			}
		}
	}

	void json::handleComment(const char*& c, size_t& s)
	{
		++c; --s;
		if (*c == '/')
		{
			do
			{
				++c; --s;
			} while (*c != '\n' && *c != 0);
		}
		else if (*c == '*')
		{
			do
			{
				++c; --s;
				if (*c == '*' && *(c + 1) == '/')
				{
					c += 2;
					s -= 2;
					break;
				}
			} while (s != 0);
		}
		else
		{
			--c; ++s;
		}
	}
}
