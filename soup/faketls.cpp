#include "faketls.hpp"
#if !SOUP_WASM || SOUP_EMSCRIPTEN

#include "WebResource.hpp"

NAMESPACE_SOUP
{
	SharedPtr<CertStore> faketls::getLatestCert()
	{
		SharedPtr<CertStore> certstore;

		WebResource cert_wr("tls.cat", "/certs/faketls-latest/cert.pem");
		WebResource key_wr("tls.cat", "/certs/faketls-latest/key.pem");
		cert_wr.downloadWithCaching();
		key_wr.downloadWithCaching();

		if (cert_wr.hasData() && key_wr.hasData())
		{
			certstore = soup::make_shared<CertStore>();
			soup::X509Certchain chainstruct;
			chainstruct.fromPem(cert_wr.data);
			certstore->add(std::move(chainstruct), soup::RsaPrivateKey::fromPem(key_wr.data));
		}

		return certstore;
	}
}
#endif
