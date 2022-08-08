#include "netAs.hpp"

#include "string.hpp"

namespace soup
{
	bool netAs::isHosting() const noexcept
	{
		switch (number)
		{
		case 4766: // Korea Telecom
		case 9009: // M247 Ltd
		case 9335: // CAT Telecom Public Company Limited
		case 12876: // ONLINE S.A.S.
		case 13046: // ISKON INTERNET d.d. za informatiku i telekomunikacije
		case 13213: // UK-2 Limited
		case 16247: // M247 Ltd
		case 20278: // Nexeon Technologies, Inc.
		case 20473: // The Constant Company, LLC
		case 25369: // Hydra Communications Ltd
		case 29485: // A1 Hrvatska d.o.o.
		case 30083: // GoDaddy.com, LLC
		case 34594: // OT - OPTIMA TELEKOM d.d.
		case 37518: // Fiber Grid INC
		case 38731: // Vietel - CHT Compamy Ltd
		case 39351: // 31173 Services AB
		case 40676: // Psychz Networks
		case 41564: // Orion Network Limited
		case 43289: // Trabia SRL
		case 45899: // VNPT Corp
		case 46805: // Inter Connects Inc
		case 47810: // Proservice LLC
		case 49981: // WorldStream B.V.
		case 51747: // Internet Vikings International AB
		case 52423: // Data Miners S.A. ( Racknation.cr )
		case 55664: // PT Inovasi Global Mumpuni
		case 58065: // Packet Exchange Limited
		case 58073: // YISP B.V.
		case 60068: // Datacamp Limited
		case 62240: // Clouvider Limited
		case 131199: // Nexeon Technologies, Inc.
		case 136787: // TEFINCOM S.A.
		case 137409: // GSL Networks Pty LTD
		case 141039: // TEFINCOM S.A.
		case 147049: // PacketHub S.A.
		case 197706: // Keminet SHPK
		case 199524: // G-Core Labs S.A.
		case 205119: // TELEKS DOOEL Skopje
		case 206804: // EstNOC OY
		case 212238: // Datacamp Limited
		case 263702: // GRUPO ZGH SPA
			// Leaseweb USA, Inc.
		case 393886:
		case 7203:
		case 19148:
		case 27411:
		case 30633:
		case 394380:
		case 395954:
		case 396190:
		case 396362:
			// Cogent Communications
		case 174:
		case 2149:
		case 4550:
		case 6259:
		case 6299:
		case 6494:
		case 6496:
		case 7061:
		case 7458:
		case 10768:
		case 11024:
		case 11220:
		case 11526:
		case 12207:
		case 16631:
		case 19164:
		case 22099:
			return true;
		}
		std::string slug = handle;
		slug.push_back(' ');
		slug.append(name);
		string::lower(slug);
		if (slug.find("cdn") != std::string::npos
			|| slug.find("colo") != std::string::npos // AS36352 ColoCrossing, AS48950 GLOBAL COLOCATION LIMITED
			|| slug.find("cloud") != std::string::npos // AS39845 2 Cloud Ltd.
			|| slug.find("datacenter") != std::string::npos
			|| slug.find("data center") != std::string::npos
			|| slug.find("data-center") != std::string::npos
			|| slug.find("ddos") != std::string::npos
			|| slug.find("dedi") != std::string::npos // AS35913 DediPath, AS42831 UK Dedicated Servers Limited
			|| slug.find("host") != std::string::npos // AS45382 EHOSTICT, AS51430 AltusHost B.V., AS55720 Gigabit Hosting Sdn Bhd, AS61493 InterBS S.R.L. (BAEHOST), AS64200 VIVID-HOSTING LLC, AS136557 Host Universal Pty Ltd, AS200698 Globalhost d.o.o., AS203020 HostRoyale Technologies Pvt Ltd, AS262287 Maxihost LTDA, AS396356 Maxihost LLC
			|| slug.find("layer") != std::string::npos // AS49453, AS57172 Global Layer B.V.
			|| slug.find("scale") != std::string::npos
			|| slug.find("server") != std::string::npos
			|| slug.find("vps") != std::string::npos
			)
		{
			return true;
		}
		return false;
	}
}
