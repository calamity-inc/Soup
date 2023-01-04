#pragma once

#include <string>
#include <unordered_map>

namespace soup
{
	class TreeReader
	{
	public:
		[[nodiscard]] virtual std::string getName(const void* node) const = 0;
		[[nodiscard]] virtual std::string getValue(const void* node) const = 0;
		[[nodiscard]] virtual bool canHaveChildren(const void* node) const { return true; }
		[[nodiscard]] virtual size_t getNumChildren(const void* node) const = 0;
		[[nodiscard]] virtual const void* getChild(const void* node, size_t i) const = 0;

		[[nodiscard]] std::string toString(const void* root, const std::string& prefix = {}) const;
		[[nodiscard]] std::string toCat(const void* root, const std::string& prefix = {}) const;

		[[nodiscard]] std::unordered_map<std::string, std::string> toMap(const void* root, bool disallow_empty_value = false) const;
	private:
		void toMap(std::unordered_map<std::string, std::string>& map, const void* root, bool disallow_empty_value, const std::string& prefix) const;
	};
}
