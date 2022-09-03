#pragma once

// Little code dump, this isn't actually used anywhere...

#include <iostream>
#include <thread>

#include <acme.hpp>
#include <Bigint.hpp>

using namespace soup;
using namespace soup::literals;

int cli_acme()
{
	AcmeClient ac("acme-staging-v02.api.letsencrypt.org");
	RsaKeypair kp(
		"119488859545982351932998564240352078493960931545479155595725520982568499587785481293538103997888423523021156584962022447620243354950306827812748170876358317706543899325796006582876822594157970180699530872182678902765149365487722250290472877016363939215144169276997130421824096832478000394765238114239556244829"_b,
		"144380238226308555133091220603986465955864013990995675033518230010874376811912725365683722385533259996477806943234737684453796099674011441829760862446777971347303240081432355893989804152889024835391549986147899235117026355394907592018230209303303004956190110517897758965278950742094402913941151883538456565673"_b
	);

	//auto acct = ac.getAccount(kp, "acme.test.1@sai.navy");
	AcmeAccount acct{ "https://acme-staging-v02.api.letsencrypt.org/acme/acct/57924204", kp.getPrivate() };

	std::cout << "Creating keypair for certificate that is to be issued...\n";
	auto certpriv = RsaKeypair::generate(2048);
	std::cout << "p = " << certpriv.p << "\n";
	std::cout << "q = " << certpriv.q << "\n";

	std::cout << "Creating order...\n";
	auto order = ac.createOrder(acct, { "example.com" });
	std::cout << "Order created at " << order.uri.toString() << "\n";
	for (const auto& auth : order.authorisations)
	{
		auto authz = ac.getAuthorisation(acct, auth);
		bool had_challenge = false;
		for (const auto& challenge : authz.challenges)
		{
			if (challenge.isDns())
			{
				had_challenge = true;
				std::string domain = "_acme-challenge.";
				domain.append(authz.domain);
				std::cout << "Deploy a TXT record at " << domain << " with the following content: " << challenge.getDnsValue(acct) << "\n";
				std::cout << "Press any key to continue.\n";
				{ std::string line; std::getline(std::cin, line); }
				ac.requestChallengeValidation(acct, challenge.uri);
			}
		}
		if (!had_challenge)
		{
			std::cout << "No DNS challenge available for " << authz.domain << "\n";
			return 1;
		}
	}
	std::cout << "Waiting until order is ready...\n";
	do
	{
		order = ac.getOrder(acct, order.uri);
		std::cout << "Order status: " << order.status << "\n";
		std::this_thread::sleep_for(std::chrono::seconds(5));
	} while (order.status == "pending");
	if (order.status == "ready")
	{
		std::cout << "Finalising order...\n";
		ac.finaliseOrder(acct, order, certpriv.getPrivate());
		std::cout << ac.getCertchain(acct, order);
	}
	return 0;
}
