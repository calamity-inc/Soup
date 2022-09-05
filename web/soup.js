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
			soup.QrCode = {
				encodeText: soup.cwrap("QrCode_encodeText", "number", ["string"]),
				free: soup.cwrap("QrCode_free", "number", ["number"]),
				toSvg: soup.cwrap("QrCode_toSvg", "string", ["number", "number", "bool", "number"]),
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
				let qr = soup.QrCode.encodeText(text);
				elm.style.display = "none";
				pruneContainer(div);
				div.innerHTML += soup.QrCode.toSvg(qr, 4, elm.hasAttribute("inverted"), 4);
				soup.QrCode.free(qr);
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
