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
			soup.base64 = {
				encode: soup.cwrap("base64_encode", "string", ["number"]),
			};
			soup.Canvas = {
				free: soup.cwrap("Canvas_free", "void", ["number"]),
				getWidth: soup.cwrap("Canvas_getWidth", "number", ["number"]),
				getHeight: soup.cwrap("Canvas_getHeight", "number", ["number"]),
				resizeNearestNeighbour: soup.cwrap("Canvas_resizeNearestNeighbour", "number", ["number", "number", "number"]),
				toSvg: soup.cwrap("Canvas_toSvg", "string", ["number", "number"]),
				toNewPngString: soup.cwrap("Canvas_toNewPngString", "number", ["number"]),
			};
			soup.QrCode = {
				free: soup.cwrap("QrCode_free", "number", ["number"]),
				newFromText: soup.cwrap("QrCode_newFromText", "number", ["string"]),
				toNewCanvas: soup.cwrap("QrCode_toNewCanvas", "number", ["number", "number", "bool"]),
			};
			soup.string = {
				free: soup.cwrap("string_free", "number", ["number"]),	
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
				let w = soup.Canvas.getWidth(c);
				let h = soup.Canvas.getWidth(c);
				soup.Canvas.resizeNearestNeighbour(c, w * 4, h * 4);
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
