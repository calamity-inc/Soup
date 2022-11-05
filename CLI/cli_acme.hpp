#pragma once

// Little code dump, this isn't actually used anywhere...

#include <iostream>

#include <acme.hpp>

using namespace soup;

int cli_acme()
{
	AcmeClient ac(AcmeClient::LETS_ENCRYPT_STAGING);
	
	std::cout << "Setting up ACME account...";
	AcmeAccount acct;
	if (auto opt = ac.discoverAccount(); opt.has_value())
	{
		acct = std::move(*opt);
		std::cout << " Reused existing one.\n";
	}
	else
	{
		acct = ac.createAndSaveAccount();
		std::cout << " Created new one and saved it.\n";
	}

	std::cout << "Creating keypair for certificate that is to be issued...\n";
	auto certkp = RsaKeypair::generate(2048);
	std::cout << "p = " << certkp.p << "\n";
	std::cout << "q = " << certkp.q << "\n";

	std::cout << ac.cliCreateCertificate(acct, certkp.getPrivate(), { "example.com" });

	return 0;
}
