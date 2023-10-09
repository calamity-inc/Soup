require "soup-apigen"

soup.tryCatch(function()
	local sched = soup.DetachedScheduler.new()
	local web_srv = soup.ServerWebService.new()
	soup.Server.bind(sched, 80, web_srv)
	while true do
		if web_srv:hasPendingRequest() then
			if web_srv:getPendingRequestPath() == "/" then
				web_srv:replyWithHtml("Hello, world!")
			else
				web_srv:replyWith404()
			end
		end
		os.sleep(1)
	end
end)
