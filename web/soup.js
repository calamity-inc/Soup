(function()
{
	var soup_q = [];

	window.soup = {
		ready: false,
		use: function(f)
		{
			soup_q.push(f);
		}
	};

	var have_libsoup_js = function()
	{
		libsoup().then(function(soup)
		{
			soup.asn1_sequence = {
				new: soup.cwrap("asn1_sequence_new", "number", ["number"]),
				free: soup.cwrap("asn1_sequence_free", "void", ["number"]),
				toDer: soup.cwrap("asn1_sequence_toDer", "number", ["number"]),
				toString: soup.cwrap("asn1_sequence_toString", "string", ["number"]),
			};
			soup.base64 = {
				encode: soup.cwrap("base64_encode", "string", ["number"]),
				decode: soup.cwrap("base64_decode", "number", ["string"]),
			};
			soup.bigint = {
				newFromString: soup.cwrap("bigint_newFromString", "number", ["string"]),
				newCopy: soup.cwrap("bigint_newCopy", "number", ["number"]),
				free: soup.cwrap("bigint_free", "void", ["number"]),
				plus: soup.cwrap("bigint_plus", "number", ["number", "number"]),
				plusEq: soup.cwrap("bigint_plusEq", "void", ["number", "number"]),
				toString: soup.cwrap("bigint_toString", "string", ["number"]),
			};
			soup.pem = {
				decode: soup.cwrap("pem_decode", "number", ["string"]),
			};
			soup.rsa = {
				keypair: {
					random: soup.cwrap("rsa_keypair_random", "number", ["number"]),
					free: soup.cwrap("rsa_keypair_free", "void", ["number"]),
					getPrivate: soup.cwrap("rsa_keypair_getPrivate", "number", ["number"]),
				},
				key_private: {
					free: soup.cwrap("rsa_key_private_free", "void", ["number"]),
					toAsn1: soup.cwrap("rsa_key_private_toAsn1", "number", ["number"]),
					toPem: soup.cwrap("rsa_key_private_toPem", "string", ["number"]),
				},
			};
			soup.string = {
				val: soup.cwrap("string_val", "string", ["number"]),
				free: soup.cwrap("string_free", "void", ["number"]),
			};
			delete soup.cwrap;
			soup.ready = true;
			soup.use = function(f)
			{
				f();
			};
			window.soup = soup;

			soup_q.forEach(function(f)
			{
				f();
			});
		});
	};

	if("libsoup" in window)
	{
		have_libsoup_js();
	}
	else
	{
		var script = document.createElement("script");
		script.src = document.currentScript.src.replace("/soup.js","/libsoup.js");
		script.onload = have_libsoup_js;
		document.currentScript.parentNode.appendChild(script);
	}
})();
