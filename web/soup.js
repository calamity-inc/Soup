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
			{
				soup.beginScope = soup.cwrap("beginScope", "void", []);
				soup.endScope = soup.cwrap("endScope", "void", []);
				soup.broadenScope = soup.cwrap("broadenScope", "void", ["number"]);
				soup.free = soup.cwrap("endLifetime", "void", ["number"]);

				soup.scope = function(f)
				{
					soup.beginScope();
					f();
					soup.endScope();
				};
			}
			soup.base40 = {
				encode: soup.cwrap("base40_encode", "string", ["number"]),
			};
			soup.base64 = {
				encode: soup.cwrap("base64_encode", "string", ["number"]),
			};
			soup.Bigint = {
				toString: soup.cwrap("Bigint_toString", "string", ["number"]),
			};
			soup.Canvas = {
				free: soup.free,
				getWidth: soup.cwrap("Canvas_getWidth", "number", ["number"]),
				getHeight: soup.cwrap("Canvas_getHeight", "number", ["number"]),
				resizeNearestNeighbour: soup.cwrap("Canvas_resizeNearestNeighbour", "number", ["number", "number", "number"]),
				toSvg: soup.cwrap("Canvas_toSvg", "string", ["number", "number"]),
				toNewPngString: soup.cwrap("Canvas_toNewPngString", "number", ["number"]),

				upscaleMultiply: function(c, f)
				{
					let w = soup.Canvas.getWidth(c);
					let h = soup.Canvas.getWidth(c);
					soup.Canvas.resizeNearestNeighbour(c, w * f, h * f);
				}
			};
			soup.InquiryLang = {
				execute: soup.cwrap("InquiryLang_execute", "number", ["string"]),
				formatResultLine: soup.cwrap("InquiryLang_formatResultLine", "string", ["number"]),
			};
			soup.InquiryObject = {
				isCanvas: soup.cwrap("InquiryObject_isCanvas", "bool", ["number"]),
				getCanvas: soup.cwrap("InquiryObject_getCanvas", "number", ["number"]),
			};
			soup.KeyGenId = {
				free: soup.free,
				generate: soup.cwrap("KeyGenId_generate", "number", []),
				toBinary: soup.cwrap("KeyGenId_toBinary", "number", ["number"]),
				getKeypair: soup.cwrap("KeyGenId_getKeypair", "number", ["number", "number"]),
			};
			soup.Mixed = {
				free: soup.free,
				isInquiryObject: soup.cwrap("Mixed_isInquiryObject", "bool", ["number"]),
				getInquiryObject: soup.cwrap("Mixed_getInquiryObject", "number", ["number"]),
			};
			soup.QrCode = {
				free: soup.free,
				newFromText: soup.cwrap("QrCode_newFromText", "number", ["string"]),
				toNewCanvas: soup.cwrap("QrCode_toNewCanvas", "number", ["number", "number", "bool"]),
			};
			soup.exception = {
				what: soup.cwrap("exception_what", "string", ["number"]),
			};
			soup.string = {
				free: soup.free,
			};
			soup.RsaKeypair = {
				getN: soup.cwrap("RsaKeypair_getN", "number", ["number"]),
				getP: soup.cwrap("RsaKeypair_getP", "number", ["number"]),
				getQ: soup.cwrap("RsaKeypair_getQ", "number", ["number"]),
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
				let qr = soup.QrCode.newFromText(text);
				let c = soup.QrCode.toNewCanvas(qr, 4, elm.hasAttribute("inverted"));
				soup.Canvas.upscaleMultiply(c, 4);
				let ps = soup.Canvas.toNewPngString(c);
				let pb = soup.base64.encode(ps);

				elm.style.display = "none";
				pruneContainer(div);
				let img = document.createElement("img");
				img.src = "data:image/png;base64," + pb;
				img.setAttribute("alt", text);
				div.appendChild(img);

				soup.string.free(ps);
				soup.Canvas.free(c);
				soup.QrCode.free(qr);
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
