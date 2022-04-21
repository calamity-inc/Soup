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
				random: soup.cwrap("bigint_random", "number", ["number"]),
				randomProbablePrime: soup.cwrap("bigint_randomProbablePrime", "number", ["number"]),
				newCopy: soup.cwrap("bigint_newCopy", "number", ["number"]),
				free: soup.cwrap("bigint_free", "void", ["number"]),
				plus: soup.cwrap("bigint_plus", "number", ["number", "number"]),
				plusEq: soup.cwrap("bigint_plusEq", "void", ["number", "number"]),
				toString: soup.cwrap("bigint_toString", "string", ["number"]),
			};
			soup.pem = {
				decode: soup.cwrap("pem_decode", "number", ["string"]),
			};
			soup.qr_code = {
				encodeText: soup.cwrap("qr_code_encodeText", "number", ["string"]),
				free: soup.cwrap("qr_code_free", "number", ["number"]),
				toSvg: soup.cwrap("qr_code_toSvg", "string", ["number", "number", "bool", "number"]),
			};
			soup.rsa = {
				keypair: {
					random: bits => new Promise(r => {
						bits /= 2;
						let pp = new Promise(r => {
							r(soup.bigint.randomProbablePrime(bits))
						});
						let pq = new Promise(r => {
							r(soup.bigint.randomProbablePrime(bits))
						});
						pp.then(p => pq.then(q => {
							r(soup.rsa.keypair.new(p, q));
						}));
					}),
					new: soup.cwrap("rsa_keypair_new", "number", ["number", "number"]),
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

			findElements();
			enableObserver();

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

	function getContainer(elm)
	{
		if(elm.parentNode.className == "soup-container")
		{
			return elm.parentNode;
		}
		let div = document.createElement("div");
		div.className = "soup-container";
		elm.parentNode.insertBefore(div, elm);
		div.appendChild(elm);
		return div;
	}

	function pruneContainer(div)
	{
		let elm = div.removeChild(div.children[0]);
		div.innerHTML = "";
		div.appendChild(elm);
	}

	function findElements()
	{
		document.querySelectorAll("soup-qr-code").forEach(elm => {
			let div = getContainer(elm);
			let svg = div.querySelector("svg");
			let text = elm.textContent;
			if(svg === null || svg.getAttribute("alt") != text)
			{
				let qr = soup.qr_code.encodeText(text);
				elm.style.display = "none";
				pruneContainer(div);
				div.innerHTML += soup.qr_code.toSvg(qr, 4, elm.hasAttribute("inverted"), 4);
				soup.qr_code.free(qr);
				div.querySelector("svg").setAttribute("alt", text);
			}
		});
	}

	var observer = new MutationObserver(function(mutations)
	{
		disableObserver();
		findElements();
		enableObserver();
	});

	function enableObserver()
	{
		observer.observe(document, {attributes: true, childList: true, characterData: true, subtree: true });
	}

	function disableObserver()
	{
		observer.disconnect();
	}
})();
