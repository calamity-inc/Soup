#include "netAs.hpp"

#include "netIntel.hpp"
#include "string.hpp"
#include "wasm.hpp"

NAMESPACE_SOUP
{
	// Checks the ASN and slug against https://github.com/calamity-inc/soup-extra-data/blob/senpai/index.blume
	bool netAs::isHosting(const netIntel& intel) const SOUP_EXCAL
	{
		WasmScript ws;
		SOUP_IF_LIKELY (ws.load(intel.extra_wasm))
		{
			if (auto code = ws.getExportedFuntion("is_hosting_asn"))
			{
				WasmVm vm(ws);
				vm.locals.emplace_back(this->number);
				if (vm.run(*code)
					&& vm.stack.back().i32
					)
				{
					return true;
				}
			}

			if (auto code = ws.getExportedFuntion("is_hosting_slug"))
			{
				std::string slug = this->handle;
				slug.push_back(' ');
				slug.append(this->name);
				string::lower(slug);

				WasmScrapAllocator sa(ws);
				auto scrap = sa.allocate(slug.size() + 1);
				if (ws.memory.write(scrap, slug.c_str(), slug.size() + 1))
				{
					WasmVm vm(ws);
					ws.memory.encodeUPTR(vm.locals.emplace_back(), scrap);
					if (vm.run(*code)
						&& vm.stack.back().i32
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
