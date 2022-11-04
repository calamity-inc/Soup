#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include <optional>
#include <string>

#include "rsa.hpp"
#include "Uri.hpp"

namespace soup
{
	struct AcmeAccount
	{
		std::string url;
		RsaPrivateKey priv;
	};

	struct AcmeOrder
	{
		Uri uri;
		std::string status;
		std::vector<std::string> domains{};
		std::vector<Uri> authorisations;
		Uri finalise;
		std::optional<Uri> certificate;
	};

	struct AcmeChallenge
	{
		Uri uri;
		std::string token;
		bool http;

		[[nodiscard]] bool isDns() const noexcept
		{
			return !http;
		}

		[[nodiscard]] std::string getHttpValue(const AcmeAccount& acct) const; // Write contents to file at path ("/.well-known/acme-challenge/" + token)
		[[nodiscard]] std::string getDnsValue(const AcmeAccount& acct) const; // Deploy in a TXT record at ("_acme-challenge." + domain)
	};

	struct AcmeAuthorisation
	{
		std::string domain;
		std::vector<AcmeChallenge> challenges;
	};

	struct AcmeClient
	{
		std::string domain;

		Uri uri_newNonce;
		Uri uri_newAccount;
		Uri uri_newOrder;

		std::string nonce;

		AcmeClient(std::string _domain);

		// ACME accounts are bound to the key, so if you call this with a registered key, the existing account is returned.
		// Let's Encrypt requires a 2048-bit or 4096-bit keypair.
		// Email address is optional to get expiry notifications.
		[[nodiscard]] AcmeAccount createAccount(const RsaKeypair& kp, std::string email = {});

		[[nodiscard]] AcmeOrder createOrder(const AcmeAccount& acct, const std::vector<std::string>& domains);
		[[nodiscard]] AcmeOrder getOrder(const AcmeAccount& acct, const Uri& uri);
		[[nodiscard]] AcmeAuthorisation getAuthorisation(const AcmeAccount& acct, const Uri& uri);
		void requestChallengeValidation(const AcmeAccount& acct, const Uri& uri);
		void finaliseOrder(const AcmeAccount& acct, AcmeOrder& order, const RsaPrivateKey& priv); // Certificate key must differ from ACME account key.
		[[nodiscard]] std::string getCertchain(const AcmeAccount& acct, const AcmeOrder& order);

		[[nodiscard]] std::string getNonce();
		HttpResponse executeRequest(const Uri& uri, std::string payload);
		HttpResponse executeRequest(const AcmeAccount& acct, const Uri& uri, const std::string& payload);
		[[nodiscard]] AcmeOrder parseOrderResponse(const HttpResponse& res);
	};
}

#endif
