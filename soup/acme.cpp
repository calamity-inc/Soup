#include "acme.hpp"

#include "base64.hpp"
#include "json.hpp"
#include "JsonArray.hpp"
#include "JsonObject.hpp"
#include "JsonString.hpp"
#include "rsa.hpp"
#include "sha256.hpp"

namespace soup
{
	std::string acme::getNewAcctPayload(const RsaKeypair& kp, const std::string& nonce, std::string email)
	{
		// This works, but only against let's encrypt staging.
		// Must be POST'd to https://acme-staging-v02.api.letsencrypt.org/acme/new-acct
		// with "Content-Type: application/jose+json"

		// I've already created an account as acme.test.1@sai.navy
		// https://acme-staging-v02.api.letsencrypt.org/acme/acct/57924204
		// p: 119488859545982351932998564240352078493960931545479155595725520982568499587785481293538103997888423523021156584962022447620243354950306827812748170876358317706543899325796006582876822594157970180699530872182678902765149365487722250290472877016363939215144169276997130421824096832478000394765238114239556244829
		// q: 144380238226308555133091220603986465955864013990995675033518230010874376811912725365683722385533259996477806943234737684453796099674011441829760862446777971347303240081432355893989804152889024835391549986147899235117026355394907592018230209303303004956190110517897758965278950742094402913941151883538456565673

		auto obj = soup::make_unique<JsonObject>();

		auto protected_obj = soup::make_unique<JsonObject>();
		protected_obj->add("alg", "RS256");
		protected_obj->add(soup::make_unique<JsonString>("jwk"), kp.getPublic().toJwk());
		protected_obj->add("nonce", nonce);
		protected_obj->add("url", "https://acme-staging-v02.api.letsencrypt.org/acme/new-acct");
		auto protected_str = base64::urlEncode(protected_obj->encode());
		obj->add("protected", protected_str);

		auto payload_obj = soup::make_unique<JsonObject>();
		payload_obj->add("termsOfServiceAgreed", true);
		auto contact_arr = soup::make_unique<JsonArray>();
		email.insert(0, "mailto:");
		contact_arr->children.emplace_back(soup::make_unique<JsonString>(std::move(email)));
		payload_obj->add(soup::make_unique<JsonString>("contact"), std::move(contact_arr));
		auto payload_str = base64::urlEncode(payload_obj->encode());
		obj->add("payload", payload_str);

		protected_str.push_back('.');
		protected_str.append(payload_str);

		obj->add("signature", base64::urlEncode(kp.getPrivate().sign<sha256>(protected_str).toBinary()));

		return obj->encode();
	}
}
