#include <iostream>
#include <thread>

#include <server.hpp>

struct SimpleClient : public soup::client
{
	bool endless = false;

	void httpClose()
	{
		auto x = recv(1024);
		close();
	}

	void sendHtml(std::string body)
	{
		auto len = body.size();
		body.insert(0, "\r\n\r\n");
		body.insert(0, std::to_string(len));
		body.insert(0, "HTTP/1.0 200\r\nContent-Length: ");
		send(body);
		httpClose();
	}
};

int main()
{
	soup::server<SimpleClient> srv;
	if (!srv.init(80))
	{
		std::cout << "Init failed. Is port 80 available?" << std::endl;
		return 1;
	}
	std::cout << "Listening on *:80..." << std::endl;
	srv.on_client_connect = [](SimpleClient& client)
	{
		std::cout << client.peer.toString()  << " + connected" << std::endl;
	};
	srv.on_client_disconnect = [](SimpleClient& client)
	{
		std::cout << client.peer.toString() << " - disconnected" << std::endl;
	};
	srv.on_client_data_available = [](SimpleClient& client)
	{
		if (client.endless)
		{
			client.send("THE END IS NEVER ");
			return;
		}
		auto buf = client.recv(50);
		auto i = buf.find(' ');
		if (i == std::string::npos)
		{
			client.send("HTTP/1.0 400\r\n\r\n");
			client.httpClose();
			return;
		}
		buf.erase(0, i + 1);
		i = buf.find(' ');
		if (i == std::string::npos)
		{
			client.send("HTTP/1.0 400\n\r\n");
			client.httpClose();
			return;
		}
		buf.erase(i);
		std::cout << client.peer.toString() << " > " << buf << std::endl;
		if (buf == "/")
		{
			client.sendHtml(R"EOC(<html>
<head>
	<title>Soup</title>
</head>
<body>
	<h1>Soup</h1>
	<p>Soup is a C++ framework that is currently private.</p>
	<p>The website you are currently viewing is directly delivered to you via a relatively simple server, using Soup's powerful abstractions. (Soon it will support TLS -- no external libraries required!)</p>
	<ul>
		<li><a href="pem-decoder">PEM Decoder</a> - Using Soup's JS API, powered by WASM.</li>
		<li><a href="endless">Endless</a> - May crash your browser.</li>
	</ul>
</body>
</html>
)EOC");
		}
		else if (buf == "/pem-decoder")
		{
			client.sendHtml(R"EOC(<html>
<head>
	<title>PEM Decoder | Soup</title>
</head>
<body>
	<h1>PEM Decoder</h1>
	<textarea oninput="processInput(event)"></textarea>
	<pre></pre>
	<script src="https://use.soup.do"></script>
	<script>
		function processInput(e)
		{
			if(soup.ready)
			{
				var seq = soup.asn1_sequence.new(soup.pem.decode(e.target.value));
				document.querySelector("pre").textContent = soup.asn1_sequence.toString(seq);
				soup.asn1_sequence.free(seq);
			}
		}
	</script>
</body>
</html>
)EOC");
		}
		else if (buf == "/endless")
		{
			client.send("HTTP/1.0 200\r\nContent-Type: text/plain\r\n\r\n");
			client.endless = true;
		}
		else
		{
			client.send("HTTP/1.0 404\r\n\r\n");
			client.httpClose();
		}
	};
	srv.run();
}
