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
			soup.beginScope = soup.cwrap("beginScope", "void", []);
			soup.endScope = soup.cwrap("endScope", "void", []);
			soup.broadenScope = soup.cwrap("broadenScope", "void", ["number"]);
			soup.free = soup.cwrap("endLifetime", "void", ["number"]);
			soup.tryCatch = soup.cwrap("tryCatch", "string", ["function"]);
			soup.throwException = soup.cwrap("throwException", "void", ["string"]);
			soup.getCountryName = soup.cwrap("getCountryName", "string", ["string", "string"]);
			soup.base32 = {
				encode: soup.cwrap("base32_encode", "string", ["number", "boolean"]),
				decode: soup.cwrap("base32_decode", "number", ["string"]),
			};
			soup.base40 = {
				encode: soup.cwrap("base40_encode", "string", ["number"]),
				decode: soup.cwrap("base40_decode", "number", ["string"]),
			};
			soup.base64 = {
				encode: soup.cwrap("base64_encode", "string", ["number"]),
			};
			soup.Bigint = {
				toString: soup.cwrap("Bigint_toString", "string", ["number"]),
			};
			soup.Canvas = {
				getWidth: soup.cwrap("Canvas_getWidth", "number", ["number"]),
				getHeight: soup.cwrap("Canvas_getHeight", "number", ["number"]),
				resizeNearestNeighbour: soup.cwrap("Canvas_resizeNearestNeighbour", "void", ["number", "number", "number"]),
				toSvg: soup.cwrap("Canvas_toSvg", "string", ["number", "number"]),
				toNewPngString: soup.cwrap("Canvas_toNewPngString", "number", ["number"]),
			};
			soup.cbResult = {
				getResponse: soup.cwrap("cbResult_getResponse", "string", ["number"]),
				isDelete: soup.cwrap("cbResult_isDelete", "boolean", ["number"]),
				getDeleteNum: soup.cwrap("cbResult_getDeleteNum", "number", ["number"]),
			};
			soup.Chatbot = {
				process: soup.cwrap("Chatbot_process", "number", ["string"]),
			};
			soup.CidrSubnetInterface = {
				new: soup.cwrap("CidrSubnetInterface_new", "number", ["string"]),
				contains: soup.cwrap("CidrSubnetInterface_contains", "boolean", ["number", "string"]),
			};
			soup.DetachedScheduler = {
				new: soup.cwrap("DetachedScheduler_new", "number", []),
				isActive: soup.cwrap("DetachedScheduler_isActive", "boolean", ["number"]),
			};
			soup.EstablishWebSocketConnectionTask = {
				new: soup.cwrap("EstablishWebSocketConnectionTask_new", "number", ["string"]),
				getSocket: soup.cwrap("EstablishWebSocketConnectionTask_getSocket", "number", ["number"]),
			};
			soup.Hotp = {
				generateSecret: soup.cwrap("Hotp_generateSecret", "number", ["number"]),
			};
			soup.HttpRequest = {
				new: soup.cwrap("HttpRequest_new", "number", ["string"]),
				setPayload: soup.cwrap("HttpRequest_setPayload", "void", ["number", "string"]),
			};
			soup.HttpRequestTask = {
				newFromRequest: soup.cwrap("HttpRequestTask_newFromRequest", "number", ["number"]),
				newFromUrl: soup.cwrap("HttpRequestTask_newFromUrl", "number", ["string"]),
				getResponseBodyCStr: soup.cwrap("HttpRequestTask_getResponseBodyCStr", "string", ["number"]),
			};
			soup.InquiryLang = {
				execute: soup.cwrap("InquiryLang_execute", "number", ["string"]),
				formatResultLine: soup.cwrap("InquiryLang_formatResultLine", "string", ["number"]),
			};
			soup.KeyGenId = {
				newFromSeedsExport: soup.cwrap("KeyGenId_newFromSeedsExport", "number", ["number", "number"]),
				generate: soup.cwrap("KeyGenId_generate", "number", ["number"]),
				toSeedsExport: soup.cwrap("KeyGenId_toSeedsExport", "number", ["number"]),
				getKeypair: soup.cwrap("KeyGenId_getKeypair", "number", ["number"]),
			};
			soup.MimeMessage = {
				addHeader: soup.cwrap("MimeMessage_addHeader", "void", ["number", "string", "string"]),
			};
			soup.Mixed = {
				isCanvas: soup.cwrap("Mixed_isCanvas", "boolean", ["number"]),
				getCanvas: soup.cwrap("Mixed_getCanvas", "number", ["number"]),
			};
			soup.PromiseBase = {
				isPending: soup.cwrap("PromiseBase_isPending", "boolean", ["number"]),
			};
			soup.Promise_WebSocketMessage = {
				getData: soup.cwrap("Promise_WebSocketMessage_getData", "string", ["number"]),
			};
			soup.QrCode = {
				newFromText: soup.cwrap("QrCode_newFromText", "number", ["string"]),
				toNewCanvas: soup.cwrap("QrCode_toNewCanvas", "number", ["number", "number", "boolean"]),
			};
			soup.RsaKeypair = {
				getN: soup.cwrap("RsaKeypair_getN", "number", ["number"]),
				getP: soup.cwrap("RsaKeypair_getP", "number", ["number"]),
				getQ: soup.cwrap("RsaKeypair_getQ", "number", ["number"]),
			};
			soup.Scheduler = {
				new: soup.cwrap("Scheduler_new", "number", []),
				setDontMakeReusableSockets: soup.cwrap("Scheduler_setDontMakeReusableSockets", "void", ["number"]),
				add: soup.cwrap("Scheduler_add", "void", ["number", "number"]),
				shouldKeepRunning: soup.cwrap("Scheduler_shouldKeepRunning", "boolean", ["number"]),
				tick: soup.cwrap("Scheduler_tick", "void", ["number"]),
			};
			soup.Totp = {
				new: soup.cwrap("Totp_new", "number", ["number"]),
				getQrCodeUri: soup.cwrap("Totp_getQrCodeUri", "string", ["number", "string", "string"]),
				getValue: soup.cwrap("Totp_getValue", "number", ["number"]),
			};
			soup.WebSocketConnection = {
				wsSend: soup.cwrap("WebSocketConnection_wsSend", "void", ["number", "string"]),
				wsRecv: soup.cwrap("WebSocketConnection_wsRecv", "number", ["number"]),
			};
			soup.Worker = {
				isWorkDone: soup.cwrap("Worker_isWorkDone", "boolean", ["number"]),
			};
			soup.YubikeyValidator = {
				new: soup.cwrap("YubikeyValidator_new", "number", ["string", "string"]),
				validate: soup.cwrap("YubikeyValidator_validate", "string", ["number", "string"]),
			};
			soup.exception = {
				what: soup.cwrap("exception_what", "string", ["number"]),
			};

			soup.CppException = class CppException extends Error
			{
			};
			soup.fixErrorType = function(e)
			{
				if (typeof e == "number")
				{
					return new soup.CppException(soup.exception.what(e));
				}   
				return e;
			}
			soup.tryCatch = function(f)
			{
				try
				{
					f();
				}
				catch (e)
				{
					throw soup.fixErrorType(e);
				}
			};
			soup.scope = function(f)
			{
				soup.beginScope();
				try
				{
					f();
				}
				catch (e)
				{
					soup.endScope();
					throw soup.fixErrorType(e);
				}
				soup.endScope();
			};

			soup.Canvas.upscaleMultiply = function(c, f)
			{
				let w = soup.Canvas.getWidth(c);
				let h = soup.Canvas.getHeight(c);
				soup.Canvas.resizeNearestNeighbour(c, w * f, h * f);
			};

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
				soup.scope(function()
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
				});
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
