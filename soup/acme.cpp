#include "acme.hpp"

#if !SOUP_WASM

#include "Asn1Sequence.hpp"
#include "base64.hpp"
#include "HttpRequest.hpp"
#include "json.hpp"
#include "JsonArray.hpp"
#include "JsonObject.hpp"
#include "JsonString.hpp"
#include "rsa.hpp"
#include "sha256.hpp"

namespace soup
{
	std::string AcmeChallenge::getHttpValue(const AcmeAccount& acct) const
	{
		std::string auth = token;
		auth.push_back('.');
		auth.append(base64::urlEncode(acct.priv.derivePublic().getJwkThumbprint()));
		return auth;
	}

	std::string AcmeChallenge::getDnsValue(const AcmeAccount& acct) const
	{
		return base64::urlEncode(sha256::hash(getHttpValue(acct)));
	}

	AcmeClient::AcmeClient(std::string _domain)
		: domain(std::move(_domain))
	{
		HttpRequest req(domain, "/directory");
		auto res = req.execute();
		auto root = json::decodeForDedicatedVariable(res->body);
		JsonObject& directory = root->asObj();
		uri_newNonce = directory.at("newNonce").asStr().value;
		uri_newAccount = directory.at("newAccount").asStr().value;
		uri_newOrder = directory.at("newOrder").asStr().value;
	}

	AcmeAccount AcmeClient::createAccount(const RsaKeypair& kp, std::string email)
	{
		auto obj = soup::make_unique<JsonObject>();

		auto protected_obj = soup::make_unique<JsonObject>();
		{
			protected_obj->add("alg", "RS256");
			protected_obj->add(soup::make_unique<JsonString>("jwk"), kp.getPublic().toJwk());
			protected_obj->add("nonce", getNonce());
			protected_obj->add("url", "https://acme-staging-v02.api.letsencrypt.org/acme/new-acct");
		}
		auto protected_str = base64::urlEncode(protected_obj->encode());
		obj->add("protected", protected_str);

		auto payload_obj = soup::make_unique<JsonObject>();
		{
			payload_obj->add("termsOfServiceAgreed", true);
			auto contact_arr = soup::make_unique<JsonArray>();
			email.insert(0, "mailto:");
			contact_arr->children.emplace_back(soup::make_unique<JsonString>(std::move(email)));
			payload_obj->add(soup::make_unique<JsonString>("contact"), std::move(contact_arr));
		}
		auto payload_str = base64::urlEncode(payload_obj->encode());
		obj->add("payload", payload_str);

		protected_str.push_back('.');
		protected_str.append(payload_str);

		obj->add("signature", base64::urlEncode(kp.getPrivate().sign<sha256>(protected_str).toBinary()));

		auto res = executeRequest(uri_newAccount, obj->encode());
		AcmeAccount acct;
		acct.url = res.header_fields.at("Location");
		acct.priv = kp.getPrivate();
		return acct;
	}

	AcmeOrder AcmeClient::createOrder(const AcmeAccount& acct, const std::vector<std::string>& domains)
	{
		auto payload_obj = soup::make_unique<JsonObject>();
		{
			auto identifiers_arr = soup::make_unique<JsonArray>();

			for (const auto& domain : domains)
			{
				auto identifier = soup::make_unique<JsonObject>();
				identifier->add("type", "dns");
				identifier->add("value", domain);

				identifiers_arr->children.emplace_back(std::move(identifier));
			}

			payload_obj->add(soup::make_unique<JsonString>("identifiers"), std::move(identifiers_arr));
		}

		return parseOrderResponse(executeRequest(acct, uri_newOrder, payload_obj->encode()));
	}

	AcmeOrder AcmeClient::getOrder(const AcmeAccount& acct, const Uri& uri)
	{
		auto order = parseOrderResponse(executeRequest(acct, uri, {}));
		order.uri = uri;
		return order;
	}

	AcmeAuthorisation AcmeClient::getAuthorisation(const AcmeAccount& acct, const Uri& uri)
	{
		auto res = executeRequest(acct, uri, {});
		auto j = json::decodeForDedicatedVariable(res.body);
		JsonObject& jo = j->asObj();
		AcmeAuthorisation authz;
		authz.domain = jo.at("identifier").asObj().at("value").asStr().value;
		for (const auto& challenge : jo.at("challenges").asArr())
		{
			const auto type = challenge.asObj().at("type").asStr().value;
			const bool is_http = (type == "http-01");
			if (!is_http
				&& type != "dns-01"
				)
			{
				continue;
			}
			authz.challenges.emplace_back(AcmeChallenge{
				challenge.asObj().at("url").asStr().value,
				challenge.asObj().at("token").asStr().value,
				is_http
			});
		}
		return authz;
	}

	void AcmeClient::requestChallengeValidation(const AcmeAccount& acct, const Uri& uri)
	{
		executeRequest(acct, uri, "{}");
	}

	void AcmeClient::finaliseOrder(const AcmeAccount& acct, AcmeOrder& order, const RsaPrivateKey& priv)
	{
		auto payload = soup::make_unique<JsonObject>();
		payload->add("csr", base64::urlEncode(priv.createCsr(order.domains).toDer()));
		order = parseOrderResponse(executeRequest(acct, order.finalise, payload->encode()));
	}

	std::string AcmeClient::getCertchain(const AcmeAccount& acct, const AcmeOrder& order)
	{
		return executeRequest(acct, *order.certificate, {}).body;
	}

	std::string AcmeClient::getNonce()
	{
		if (!this->nonce.empty())
		{
			auto nonce = std::move(this->nonce);
			this->nonce.clear();
			return nonce;
		}
		HttpRequest req(uri_newNonce.host, uri_newNonce.path);
		auto res = req.execute();
		return res->header_fields.at("Replay-Nonce");
	}

	HttpResponse AcmeClient::executeRequest(const Uri& uri, std::string payload)
	{
		HttpRequest req("POST", uri.host, uri.path);
		req.header_fields.emplace("Content-Type", "application/jose+json");
		req.setPayload(std::move(payload));
		auto res = req.execute();
		nonce = res->header_fields.at("Replay-Nonce");
		return *res;
	}

	HttpResponse AcmeClient::executeRequest(const AcmeAccount& acct, const Uri& uri, const std::string& payload)
	{
		auto obj = soup::make_unique<JsonObject>();

		auto protected_obj = soup::make_unique<JsonObject>();
		{
			protected_obj->add("alg", "RS256");
			protected_obj->add("kid", acct.url);
			protected_obj->add("nonce", getNonce());
			protected_obj->add("url", uri.toString());
		}
		auto protected_str = base64::urlEncode(protected_obj->encode());
		obj->add("protected", protected_str);

		auto payload_str = base64::urlEncode(payload);
		obj->add("payload", payload_str);

		protected_str.push_back('.');
		protected_str.append(payload_str);

		obj->add("signature", base64::urlEncode(acct.priv.sign<sha256>(protected_str).toBinary()));

		return executeRequest(uri, obj->encode());
	}

	AcmeOrder AcmeClient::parseOrderResponse(const HttpResponse& res)
	{
		auto j = json::decodeForDedicatedVariable(res.body);
		JsonObject& jo = j->asObj();
		AcmeOrder order{};
		if (auto e = res.header_fields.find("Location"); e != res.header_fields.end())
		{
			order.uri = e->second;
		}
		order.status = jo.at("status").asStr().value;
		for (const auto& id : jo.at("identifiers").asArr())
		{
			order.domains.emplace_back(id.asObj().at("value").asStr().value);
		}
		for (const auto& auth : jo.at("authorizations").asArr())
		{
			order.authorisations.emplace_back(auth.asStr().value);
		}
		order.finalise = jo.at("finalize").asStr().value;
		if (auto certificate = jo.find("certificate"))
		{
			order.certificate = certificate->asStr().value;
		}
		return order;
	}
}

#endif
