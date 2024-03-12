#include "netAs.hpp"

#include "base64.hpp"
#include "netIntel.hpp"
#include "string.hpp"
#include "wasm.hpp"

namespace soup
{
	bool netAs::isHosting(const netIntel& intel) const SOUP_EXCAL
	{
		return isHosting(intel.extra_wasm);
	}

	bool netAs::isHosting() const SOUP_EXCAL
	{
		// Hard-coding https://github.com/calamity-inc/soup-extra-data/tree/2cf65d932945524f529c0bb2527df51bc5b18b92
		return isHosting(base64::decode("AGFzbQEAAAABBgFgAX8BfwMCAQAFAwEAAAcbAg5pc19ob3N0aW5nX2FzbgAABm1lbW9yeQIACqMIAaAIACAAQbElRiAAQY4ZRnIgAEHZMEZyIABBpD9GciAAQdjIAEZyIABB4MgARnIgAEH3yABGciAAQbfcAEZyIABB5twARnIgAEH25QBGciAAQZ3nAEZyIABBtp4BRnIgAEH5nwFGciAAQZnGAUZyIABBreYBRnIgAEGD6wFGciAAQYDPAUZyIABBkPABRnIgAEGulwJGciAAQY6lAkZyIABBy64CRnIgAEG3swJGciAAQeS9AkZyIABB3MQCRnIgAEGZ0gJGciAAQfnTAkZyIABBruACRnIgAEHL5gJGciAAQeLrAkZyIABB1e0CRnIgAEH87QJGciAAQcL1AkZyIABBvYYDRnIgAEGAiQNGciAAQaOUA0ZyIABBx5kDRnIgAEGVngNGciAAQcbGA0ZyIABBo6MDRnIgAEHwsgNGciAAQdHFA0ZyIABB2cUDRnIgAEGk1QNGciAAQaDmA0ZyIABB/4AIRnIgAEHriAhGciAAQeiSCEZyIABB06wIRnIgAEHBrQhGciAAQa+wCEZyIABBwbEIRnIgAEHvzQhGciAAQe/OCEZyIABBkvsIRnIgAEHp/AhGciAAQcqIDEZyIABB440MRnIgAEHNjwxGciAAQd2PDEZyIABB5JYMRnIgAEH9wQxGciAAQb/CDEZyIABB1M8MRnIgAEGO+gxGciAAQZ2CDUZyIABBlowQRnIgAEGlshBGciAAQdSwAUZyIABB8MIARnIgAEHm9QJGciAAQY7sAkZyIABBzJgCRnIgAEG8HkZyIABBx6QDRnIgAEHENEZyIABB25IBRnIgAEGAnAJGciAAQbfVA0ZyIABBiz9GciAAQbr0AkZyIABB7MwMRnIgAEGYzQhGciAAQenLAkZyIABBqNADRnIgAEG+5wxGciAAQd3SAkZyIABB6ugBRnIgAEHSrQNGciAAQdz3A0ZyIABBlxlGciAAQb+2AkZyIABBodIMRnIgAEGCgg1GciAAQa+2GEZyIABButkDRnIgAEHeiQxGciAAQdbSAkZyIABBtfcDRnIgAEGPgRBGciAAQcSYGEZyIABBrr0CRnIgAEGdwQxGciAAQfrxAEZyIABB7/8BRnIgAEHUzQJGciAAQazXAkZyIABBrgFGciAAQeUQRnIgAEHGI0ZyIABB8zBGciAAQZsxRnIgAEHeMkZyIABB4DJGciAAQZU3RnIgAEGiOkZyIABBkNQARnIgAEGQ1gBGciAAQdTXAEZyIABBhtoARnIgAEGv3wBGciAAQfeBAUZyIABB3JUBRnIgAEHTrAFGciAAQczkAEZyIABBy7sIRnIgAEG6nRhGciAAQYHDAUZyIABBxMMCRnIgAEHGnRhGciAAQbTvAUZyIABB5uwARnIgAEG7pwNGciAAQf2xAUZyIABBu+kDRnILADIEbmFtZQEXAQAUaW5kZXgvaXNfaG9zdGluZ19hc24CBgEAAQABMAQEAQABMAYEAQABMA=="));
	}

	bool netAs::isHosting(const std::string& extra_wasm) const SOUP_EXCAL
	{
		// Checking ASN against https://github.com/calamity-inc/soup-extra-data/blob/senpai/index.ts
		WasmScript ws;
		if (ws.load(extra_wasm))
		{
			if (auto code = ws.getExportedFuntion("is_hosting_asn"))
			{
				WasmVm vm;
				vm.stack.emplace(this->number);
				if (vm.run(*code)
					&& vm.stack.top()
					)
				{
					return true;
				}
			}
		}

		std::string slug = this->handle;
		slug.push_back(' ');
		slug.append(this->name);
		string::lower(slug);
		string::replaceAll(slug, "-", "");
		string::replaceAll(slug, ",", "");
		if (slug.find("cdn") != std::string::npos
			|| slug.find("colocation") != std::string::npos // AS48950 GLOBAL COLOCATION LIMITED
			// Note: Not "colo" because "Telmex Colombia S.A."
			|| (slug.find("cloud") != std::string::npos // AS39845 2 Cloud Ltd.
				&& slug.find("the cloud") == std::string::npos // ignore AS41012 The Cloud Networks Limited
				)
			|| slug.find("datacenter") != std::string::npos
			|| slug.find("data center") != std::string::npos
			|| slug.find("ddos") != std::string::npos
			|| slug.find("dedi") != std::string::npos // AS35913 DediPath, AS42831 UK Dedicated Servers Limited
			|| (slug.find("host") != std::string::npos // AS45382 EHOSTICT, AS51430 AltusHost B.V., AS55720 Gigabit Hosting Sdn Bhd, AS61493 InterBS S.R.L. (BAEHOST), AS64200 VIVID-HOSTING LLC, AS136557 Host Universal Pty Ltd, AS200698 Globalhost d.o.o., AS203020 HostRoyale Technologies Pvt Ltd
				&& slug.find("afrihost") == std::string::npos
				)
			|| slug.find("layer") != std::string::npos // AS49453, AS57172 Global Layer B.V.
			|| slug.find("server") != std::string::npos
			|| slug.find("vps") != std::string::npos
			|| slug.find("hetzner") != std::string::npos
			|| slug.find("ovh") != std::string::npos
			|| slug.find("contabo") != std::string::npos
			|| slug.find("digitalocean") != std::string::npos
			|| slug.find("amazon") != std::string::npos
			|| slug.find("google llc") != std::string::npos // added LLC to prevent detection of "Google Fiber Inc"
			|| slug.find("akamai") != std::string::npos
			|| slug.find("microsoft") != std::string::npos
			|| slug.find("alibaba") != std::string::npos
			|| slug.find("fastly") != std::string::npos
			|| slug.find("linode") != std::string::npos
			|| slug.find("aruba") != std::string::npos
			|| slug.find("godaddy") != std::string::npos
			|| slug.find("oracle") != std::string::npos
			|| slug.find("m247") != std::string::npos
			|| slug.find("leaseweb") != std::string::npos
			|| slug.find("ipxo") != std::string::npos // AS61317, AS206092
			)
		{
			return true;
		}
		return false;
	}
}
