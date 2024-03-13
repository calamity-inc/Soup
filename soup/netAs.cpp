#include "netAs.hpp"

#include "netIntel.hpp"
#include "string.hpp"
#include "wasm.hpp"

namespace soup
{
	bool netAs::isHosting(const netIntel& intel) const SOUP_EXCAL
	{
		WasmScript ws;
		SOUP_IF_LIKELY (ws.load(intel.extra_wasm))
		{
			// Checking ASN against https://github.com/calamity-inc/soup-extra-data/blob/senpai/src/is_hosting_asn.ts
			if (auto code = ws.getExportedFuntion("is_hosting_asn"))
			{
				WasmVm vm(ws);
				vm.locals.emplace_back(this->number);
				if (vm.run(*code)
					&& vm.stack.top()
					)
				{
					return true;
				}
			}

			// Checking slug against https://github.com/calamity-inc/soup-extra-data/blob/senpai/src/is_hosting_slug.c
			if (auto code = ws.getExportedFuntion("is_hosting_slug"))
			{
				std::string slug = this->handle;
				slug.push_back(' ');
				slug.append(this->name);
				string::lower(slug);

				auto scrap = ws.allocateMemory(slug.size() + 1);
				if (ws.setMemory(scrap, slug.c_str(), slug.size() + 1))
				{
					WasmVm vm(ws);
					vm.locals.emplace_back(scrap);
					if (vm.run(*code)
						&& vm.stack.top()
						)
					{
						return true;
					}
				}
			}
		}
		return false;
	}
}
