#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include <filesystem>
#include <optional>
#include <string>

#include "HttpRequestTask.hpp"
#include "Packet.hpp"
#include "rsa.hpp"
#include "Uri.hpp"

namespace soup
{
	SOUP_PACKET(AcmeAccount)
	{
		std::string url;
		RsaPrivateKey priv;

		AcmeAccount() = default;

		AcmeAccount(std::string url, RsaPrivateKey priv)
			: url(std::move(url)), priv(std::move(priv))
		{
		}

		SOUP_PACKET_IO(s)
		{
			return s.str_nt(url)
				&& priv.io(s)
				;
		}
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
		inline static std::string LETS_ENCRYPT = "acme-v02.api.letsencrypt.org";
		inline static std::string LETS_ENCRYPT_STAGING = "acme-staging-v02.api.letsencrypt.org";

		std::string domain;

		Uri uri_newNonce;
		Uri uri_newAccount;
		Uri uri_newOrder;

		std::string nonce;

		AcmeClient() = default; // requires init
		AcmeClient(std::string domain); // blocking

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

		[[nodiscard]] std::filesystem::path getAccountPath() const;
		[[nodiscard]] std::optional<AcmeAccount> discoverAccount() const;
		[[nodiscard]] AcmeAccount discoverOrCreateAccount();
		[[nodiscard]] AcmeAccount createAndSaveAccount();

		// Prompts the user to solve the needed DNS challenges.
		// Returns pem-encoded certchain on success. Empty string on failure.
		// Note that the certificate key must differ from ACME account key.
		[[nodiscard]] std::string cliCreateCertificate(const AcmeAccount& acct, const RsaPrivateKey& priv, const std::vector<std::string>& domains);

		struct InitTask : public Task
		{
			AcmeClient& client;
			HttpRequestTask hrt;

			InitTask(AcmeClient& client, std::string domain)
				: client(client), hrt(domain, "/directory")
			{
			}

			void onTick() final;
		};

		[[nodiscard]] InitTask init(std::string domain) { return InitTask(*this, std::move(domain)); }
	};
}

#endif
