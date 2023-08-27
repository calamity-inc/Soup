#include <iostream>

#include <crc32.hpp>
#include <HttpRequest.hpp>
#include <pem.hpp>
#include <rsa.hpp>
#include <Scheduler.hpp>
#include <Server.hpp>
#include <ServerWebService.hpp>
#include <Socket.hpp>
#include <string.hpp>
#include <TlsClientHello.hpp>
#include <TlsCipherSuite.hpp>
#include <TlsExtensionType.hpp>
#include <TlsServerRsaData.hpp>
#include <WebSocketMessage.hpp>

struct SimpleServerClientData
{
	std::vector<uint16_t> cipher_suites{};
	std::vector<uint8_t> compression_methods{};
	std::vector<uint16_t> extensions{};

	[[nodiscard]] uint32_t getTlsIdHash() const noexcept
	{
		uint32_t sum = soup::crc32::INITIAL;
		sum = soup::crc32::hash((const uint8_t*)cipher_suites.data(), cipher_suites.size() * 2, sum);
		sum = soup::crc32::hash((const uint8_t*)compression_methods.data(), compression_methods.size(), sum);
		sum = soup::crc32::hash((const uint8_t*)extensions.data(), extensions.size() * 2, sum);
		return sum;
	}
};

static void handleRequest(soup::Socket& s, soup::HttpRequest&& req, soup::ServerWebService&)
{
	std::cout << s.peer.toString() << " > " << req.method << " " << req.path << std::endl;
	if (req.path == "/")
	{
		soup::ServerWebService::sendHtml(s, R"EOC(<html>
<head>
	<title>Soup</title>
</head>
<body>
	<h1>Soup</h1>
	<p>Soup is a C++ framework that does absolutely everything, with no external dependenices.</p>
	<p>The website you are currently viewing is directly delivered to you via a relatively simple server, using Soup's powerful abstractions.</p>
	<ul>
		<li><a href="/tlsid">TLS fingerprint</a></li>
	</ul>
	<hr>
	<p><a href="https://github.com/calamity-inc/Soup">Soup on Github</a></p>
</body>
</html>
)EOC");
	}
	else if (req.path == "/tlsid")
	{
		if (!s.isEncrypted())
		{
			soup::ServerWebService::sendHtml(s, "For security reasons, we can only send you your TLS ID over a secure connection. :^)");
		}
		else
		{
			// Disable keep-alive to allow seeing randomisation on reload.
			soup::ServerWebService::disableKeepAlive(s);

			auto& data = s.custom_data.getStructFromMap(SimpleServerClientData);

			bool has_grease = false;

			std::string cipher_suites_str{};
			for (const auto& cs : data.cipher_suites)
			{
				std::string str = soup::string::hex(cs);
				if (soup::tls_isGreaseyCiphersuite(cs))
				{
					has_grease = true;
					str.insert(0, "<i>");
					str.append("</i>");
				}
				soup::string::listAppend(cipher_suites_str, std::move(str));
			}

			std::string compression_methods_str{};
			for (const auto& cm : data.compression_methods)
			{
				soup::string::listAppend(compression_methods_str, soup::string::hex(cm));
			}

			std::string extensions_str{};
			for (const auto& ext : data.extensions)
			{
				std::string str = soup::string::hex(ext);
				if (soup::tls_isGreaseyExtension(ext))
				{
					has_grease = true;
					str.insert(0, "<i>");
					str.append("</i>");
				}
				soup::string::listAppend(extensions_str, std::move(str));
			}

			std::string str = "<p>Your client offered the following for the TLS handshake:</p><ul><li>Cipher suites: ";
			str.append(cipher_suites_str);
			str.append("</li><li>Compression methods: ");
			str.append(compression_methods_str);
			str.append("</li><li>Extensions: ");
			str.append(extensions_str);
			str.append("</li></ul>");
			if (has_grease)
			{
				str.append("<p>Italic values are GREASE, which will be randomised with each request.</p>");

				for (auto i = data.cipher_suites.begin(); i != data.cipher_suites.end(); )
				{
					if (soup::tls_isGreaseyCiphersuite(*i))
					{
						i = data.cipher_suites.erase(i);
					}
					else
					{
						++i;
					}
				}
				for (auto i = data.extensions.begin(); i != data.extensions.end(); )
				{
					if (soup::tls_isGreaseyExtension(*i))
					{
						i = data.extensions.erase(i);
					}
					else
					{
						++i;
					}
				}
			}

			str.append("<p>\"Hashing\" this together ");
			if (has_grease)
			{
				str.append("(sans GREASE) ");
			}
			str.append("gives us the following: ");
			str.append(soup::string::hex(data.getTlsIdHash()));

			std::sort(data.cipher_suites.begin(), data.cipher_suites.end(), [](uint16_t a, uint16_t b)
			{
				return a < b;
			});
			std::sort(data.extensions.begin(), data.extensions.end(), [](uint16_t a, uint16_t b)
			{
				return a < b;
			});

			str.append("</p><p>Additionally, if we ignore ordering, we get this hash: ");
			str.append(soup::string::hex(data.getTlsIdHash()));
			str.append("</p>");

			// Chrome: CA5B7A48
			// Firefox: D3519C76
			// Firefox (Incognito): A0C44F97
			// cURL: 2D01D915
			// Soup: 3CCAD55E, 584CC037, B6289622, DA7F09A6

			soup::ServerWebService::sendHtml(s, std::move(str));
		}
	}
	else
	{
		soup::ServerWebService::send404(s);
	}
}

static soup::TlsServerRsaData server_rsa_data;

static void cert_selector(soup::TlsServerRsaData& out, const std::string& server_name)
{
	out = server_rsa_data;
}

static void on_client_hello(soup::Socket& s, soup::TlsClientHello&& hello)
{
	auto& data = s.custom_data.getStructFromMap(SimpleServerClientData);
	data.cipher_suites = std::move(hello.cipher_suites);
	data.compression_methods = std::move(hello.compression_methods);
	data.extensions.reserve(hello.extensions.extensions.size());
	for (const auto& ext : hello.extensions.extensions)
	{
		data.extensions.emplace_back(ext.id);
	}
}

int main()
{
	server_rsa_data.der_encoded_certchain = {
		soup::pem::decode(R"EOC(
-----BEGIN CERTIFICATE-----
MIIFEjCCA/qgAwIBAgISAzO1ak1tzkSo99OqAn2x+OfMMA0GCSqGSIb3DQEBCwUA
MDIxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQD
EwJSMzAeFw0yMjAzMDcwMjM0MzdaFw0yMjA2MDUwMjM0MzZaMBIxEDAOBgNVBAMT
B3NvdXAuZG8wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDYbeJly69N
d+bPEnzCEAJZzqp/xsGr2YTgisYGyBKqp0ubWfl4ItRx7a50siEVy57oNBc4AGhQ
6/A1+IFQ2n/kGvvY0sTLs4Pv1yRD1Qs2VkDS6Jcor5O+tzvOomHko7mJ5cvxIq15
rocdKsJGhFY7k3S+iY0pXk5niEDMts6JMrXOja9oDt5pWoDNDECfIN7D1THB+v1i
8tsS5ScFCgVpFf9fLW8sW0weeEp0mpAIO5LUdQWWRFd7csMtVorJauYy5b0qKKZV
6eUWy04OrhZL+Adqg1KT/LBvO69oZNBoCOehoVJnoqgMTfTweUTPpNGYmlNanG7J
Qu++GqrcfzRzAgMBAAGjggJAMIICPDAOBgNVHQ8BAf8EBAMCBaAwHQYDVR0lBBYw
FAYIKwYBBQUHAwEGCCsGAQUFBwMCMAwGA1UdEwEB/wQCMAAwHQYDVR0OBBYEFE7Q
uqEvBJXbxPcOKhxv6fZHEMchMB8GA1UdIwQYMBaAFBQusxe3WFbLrlAJQOYfr52L
FMLGMFUGCCsGAQUFBwEBBEkwRzAhBggrBgEFBQcwAYYVaHR0cDovL3IzLm8ubGVu
Y3Iub3JnMCIGCCsGAQUFBzAChhZodHRwOi8vcjMuaS5sZW5jci5vcmcvMBIGA1Ud
EQQLMAmCB3NvdXAuZG8wTAYDVR0gBEUwQzAIBgZngQwBAgEwNwYLKwYBBAGC3xMB
AQEwKDAmBggrBgEFBQcCARYaaHR0cDovL2Nwcy5sZXRzZW5jcnlwdC5vcmcwggEC
BgorBgEEAdZ5AgQCBIHzBIHwAO4AdQDfpV6raIJPH2yt7rhfTj5a6s2iEqRqXo47
EsAgRFwqcwAAAX9icXU+AAAEAwBGMEQCIGn3bY2k5A2Bm6Vj/MJzsu37VR7VgCK9
DGlZE1uIiJmlAiB03bP3Yzi2IMq7kZ7iTyN73jX5BjN/1nSdG10jHOP35gB1ACl5
vvCeOTkh8FZzn2Old+W+V32cYAr4+U1dJlwlXceEAAABf2JxdTAAAAQDAEYwRAIg
UpJu1XNFarHUfzbjAhb0+dZA9VlB/soZ51BJq/bIXkYCIFqCNaaLy8mXfS9E+pGs
wnHTvAAzIVZhbZc/+rzH+JY0MA0GCSqGSIb3DQEBCwUAA4IBAQBSfR9b4Wt17a8l
g1PMh0d2m7B5WMYuovOwd1A8KEOavYe2QqIQ1olS1vPKPFwtp1dk4qhqWd3vCCGf
84cCfwpnwrk1bSeT4PBy3z1hOHaZ+iFH/GXRC6B18MWueRMmM9k4McKUTjQzRatm
Am+yUiq0BvgST2Ctf/LZbKsecvTnkLPzUededMfO3s631JIFu/8Uh0gvE+daucdV
CFz9GEwFTtBOZVm2pXQa7WJZkkas2CEp8OaE1uTi0dNsjsmoucx5NjnAVIJsZYjl
fSAiz4aov7Yb7NyFQqfMAhjYQIxkoJSHwhTWRTlccEiwtPvOU+e3lYQVemWlHw7W
eG23qKPg
-----END CERTIFICATE-----
)EOC"),
soup::pem::decode(R"EOC(
-----BEGIN CERTIFICATE-----
MIIFFjCCAv6gAwIBAgIRAJErCErPDBinU/bWLiWnX1owDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjAwOTA0MDAwMDAw
WhcNMjUwOTE1MTYwMDAwWjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg
RW5jcnlwdDELMAkGA1UEAxMCUjMwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK
AoIBAQC7AhUozPaglNMPEuyNVZLD+ILxmaZ6QoinXSaqtSu5xUyxr45r+XXIo9cP
R5QUVTVXjJ6oojkZ9YI8QqlObvU7wy7bjcCwXPNZOOftz2nwWgsbvsCUJCWH+jdx
sxPnHKzhm+/b5DtFUkWWqcFTzjTIUu61ru2P3mBw4qVUq7ZtDpelQDRrK9O8Zutm
NHz6a4uPVymZ+DAXXbpyb/uBxa3Shlg9F8fnCbvxK/eG3MHacV3URuPMrSXBiLxg
Z3Vms/EY96Jc5lP/Ooi2R6X/ExjqmAl3P51T+c8B5fWmcBcUr2Ok/5mzk53cU6cG
/kiFHaFpriV1uxPMUgP17VGhi9sVAgMBAAGjggEIMIIBBDAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMBMBIGA1UdEwEB/wQIMAYB
Af8CAQAwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYfr52LFMLGMB8GA1UdIwQYMBaA
FHm0WeZ7tuXkAXOACIjIGlj26ZtuMDIGCCsGAQUFBwEBBCYwJDAiBggrBgEFBQcw
AoYWaHR0cDovL3gxLmkubGVuY3Iub3JnLzAnBgNVHR8EIDAeMBygGqAYhhZodHRw
Oi8veDEuYy5sZW5jci5vcmcvMCIGA1UdIAQbMBkwCAYGZ4EMAQIBMA0GCysGAQQB
gt8TAQEBMA0GCSqGSIb3DQEBCwUAA4ICAQCFyk5HPqP3hUSFvNVneLKYY611TR6W
PTNlclQtgaDqw+34IL9fzLdwALduO/ZelN7kIJ+m74uyA+eitRY8kc607TkC53wl
ikfmZW4/RvTZ8M6UK+5UzhK8jCdLuMGYL6KvzXGRSgi3yLgjewQtCPkIVz6D2QQz
CkcheAmCJ8MqyJu5zlzyZMjAvnnAT45tRAxekrsu94sQ4egdRCnbWSDtY7kh+BIm
lJNXoB1lBMEKIq4QDUOXoRgffuDghje1WrG9ML+Hbisq/yFOGwXD9RiX8F6sw6W4
avAuvDszue5L3sz85K+EC4Y/wFVDNvZo4TYXao6Z0f+lQKc0t8DQYzk1OXVu8rp2
yJMC6alLbBfODALZvYH7n7do1AZls4I9d1P4jnkDrQoxB3UqQ9hVl3LEKQ73xF1O
yK5GhDDX8oVfGKF5u+decIsH4YaTw7mP3GFxJSqv3+0lUFJoi5Lc5da149p90Ids
hCExroL1+7mryIkXPeFM5TgO9r0rvZaBFOvV2z0gp35Z0+L4WPlbuEjN/lxPFin+
HlUjr8gRsI3qfJOQFy/9rKIJR0Y/8Omwt/8oTWgy1mdeHmmjk7j1nYsvC9JSQ6Zv
MldlTTKB3zhThV1+XWYp6rjd5JW1zbVWEkLNxE7GJThEUG3szgBVGP7pSWTUTsqX
nLRbwHOoq7hHwg==
-----END CERTIFICATE-----
)EOC"),
	};
	server_rsa_data.private_key = soup::RsaPrivateKey::fromPem(R"EOC(
-----BEGIN PRIVATE KEY-----
MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDYbeJly69Nd+bP
EnzCEAJZzqp/xsGr2YTgisYGyBKqp0ubWfl4ItRx7a50siEVy57oNBc4AGhQ6/A1
+IFQ2n/kGvvY0sTLs4Pv1yRD1Qs2VkDS6Jcor5O+tzvOomHko7mJ5cvxIq15rocd
KsJGhFY7k3S+iY0pXk5niEDMts6JMrXOja9oDt5pWoDNDECfIN7D1THB+v1i8tsS
5ScFCgVpFf9fLW8sW0weeEp0mpAIO5LUdQWWRFd7csMtVorJauYy5b0qKKZV6eUW
y04OrhZL+Adqg1KT/LBvO69oZNBoCOehoVJnoqgMTfTweUTPpNGYmlNanG7JQu++
GqrcfzRzAgMBAAECggEBAKGQLfClw6CGAFPWTjGkN80I3PhzzAHYaDwi/D71vhGM
v4EiAnvvLD48Gv5cNxyJG3/l2utgSn8WEgSIFSjhY5VJm3W5qVUTFkvFg/nrIOqY
Kt4G6UhjAVzedhQD3iYLHqdVVxAUPgHXCl/4mnx/r8vbgMv37NvT3Z2l9hGb6cQ6
KThWdtse2/cG7bwnQQWQI/5gd/3technnzYtPh47s2Z0fj0CUp8oA9eZa2oZhmFd
5wNnX5Sy5OSZQlOJHEg/9kN9GkahtyjTQ6IGiocuU0Fb788ZF0QEZuB2ddc87lAB
hiAd1TgbieU+11idqh1C5wCLCPtAevXRhQNhdKeubSECgYEA/uOtO0jmRT6jFf1A
XtUPaJ6kWyjSA6fu/HoAsGYzxz1yiUDM0PWyXWQ8dUYe41pd6QDu++l4fkJLB6LC
Ks7hewsCqZIRw8w3ebbABqWBFW4RT3w+nYOdaJwrHUbwg+YoLrC6iJwOusC/FKr5
p6USYp38O5RE2Xb12j9F8dcJl2MCgYEA2V9OaFb+PTKCL6PHZYoeTg4ElVLHEq5E
H77dJYAd2A75amEKC3L5WqYiLTrOdBqIOhXXSUSMNd2pEecw6u50A5vI0ezJP40/
bEYQDwpQDhdtvhWPtLJ1C0GsCyjgHwpmcEAD3rNB6M0nIROD/BKIaABePD7KoOu8
yEbdWJeZI7ECgYBEaWt3fAuCDlvLbRu32Eu4csv+Q6iKnqpATaadsfC3y0BQonnW
o/tpoZuwhk+IChsmjL+YEYPrr3Nf60leIATY942RYcku2kMRggFsR0OsMsymntxX
fpnjF/didkXbwQyL65dFT02MxmsC6xjy7BVRLsIiY5tPGuTF3TGyxVqnrQKBgQC6
82IvEOq2XXNkX7rFlMW9ogbFGp2GboS+vNvcPdTtFuviVzVZZXgaQ5pPRi1747nY
IyK2rBLe3RZlBG6pD46N7/UGv1zSoLu0domnNdpmVDYZbtfatEU/+ipqqqwfZkV2
M0hgx9Fe1NrbcrpoGNRihjaGIAcL4dPKeFA0uqWF8QKBgFN4K7Veh7E8nJ8D/ErB
AA3w0djptVzPMBF7JFE72qgIYiErviEf2X5aFwTLQJNtpLBNDFGhlsyM0B3vQMG6
IWTRPUZRNojVvK1dQ+xPN/9HsFVUb6JWyU4e3gocnYoe2zGdyT9p9u0Pr3JikgAC
QJg24g1I/Zb4EUJmo2WNBzGS
-----END PRIVATE KEY-----
)EOC");

	soup::Server serv{};
	serv.on_work_done = [](soup::Worker& w, soup::Scheduler&)
	{
		std::cout << reinterpret_cast<soup::Socket&>(w).peer.toString() << " - work done" << std::endl;
	};
	serv.on_connection_lost = [](soup::Socket& s, soup::Scheduler&)
	{
		std::cout << s.peer.toString() << " - connection lost" << std::endl;
	};
	serv.on_exception = [](soup::Worker& w, const std::exception& e, soup::Scheduler&)
	{
		std::cout << reinterpret_cast<soup::Socket&>(w).peer.toString() << " - exception: " << e.what() << std::endl;
	};

	soup::ServerWebService web_srv{ &handleRequest };
	web_srv.on_connection_established = [](soup::Socket& s, soup::ServerService&, soup::Server&)
	{
		std::cout << s.peer.toString() << " + connection established" << std::endl;
	};
	web_srv.should_accept_websocket_connection = [](soup::Socket& s, const soup::HttpRequest& req, soup::ServerWebService&)
	{
		std::cout << s.peer.toString() << " > WEBSOCKET " << req.path << std::endl;
		return true;
	};
	web_srv.on_websocket_message = [](soup::WebSocketMessage& msg, soup::Socket& s, soup::ServerWebService&)
	{
		soup::ServerWebService::wsSend(s, msg.data, msg.is_text);
	};

	if (!serv.bind(80, &web_srv))
	{
		std::cout << "Failed to bind to port 80." << std::endl;
		return 1;
	}
	if (!serv.bindCrypto(443, &web_srv, &cert_selector, &on_client_hello))
	{
		std::cout << "Failed to bind to port 443." << std::endl;
		return 2;
	}
	std::cout << "Listening on ports 80 and 443." << std::endl;
	serv.run();
}
