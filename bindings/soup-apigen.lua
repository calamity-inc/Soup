-- See also: https://github.com/calamity-inc/Soup-Lua-Bindings

local ffi = require("luaffi") -- https://github.com/calamity-inc/luaffi
if not SOUP_WORKING_DIR then
	SOUP_WORKING_DIR = ""
end
local libsoup = ffi.open(SOUP_WORKING_DIR .. "soup")

local function initClass(mt, t)
	setmetatable(t, mt)
	mt.__index = mt
	return t
end

soup = {
	tryCatch = function(f)
		local err = libsoup:callString("tryCatch", function()
			f()
		end)
		if err ~= nil then
			error(err)
		end
	end,
	Hotp = {
		generateSecret = function(bytes)
			if bytes == nil then
				bytes = 16
			end
			return initClass(soup.stdstring, { addr = libsoup:call("Hotp_generateSecret", bytes) })
		end,
	},
	Server = {
		bind = function(serv, port, srv)
			assert(getmetatable(serv) == soup.Scheduler or getmetatable(serv) == soup.DetachedScheduler)
			assert(getmetatable(srv) == soup.ServerService or getmetatable(srv) == soup.ServerWebService)
			return 0 ~= libsoup:call("Server_bind", serv.addr, port, srv.addr)
		end,
	},
	stdstring = {
		__name = "soup.stdstring",
		__gc = function(self)
			libsoup:call("endLifetime", self.addr)
		end,
	},
	DetachedScheduler = {
		__name = "soup.DetachedScheduler",
		__gc = function(self)
			libsoup:call("endLifetime", self.addr)
		end,
		new = function()
			return initClass(soup.DetachedScheduler, { addr = libsoup:call("DetachedScheduler_new") })
		end,
		isActive = function(self)
			return 0 ~= libsoup:call("DetachedScheduler_isActive", self.addr)
		end,
		setDontMakeReusableSockets = function(self)
			libsoup:call("Scheduler_setDontMakeReusableSockets", self.addr)
		end,
		add = function(self, spWorker)
			assert(getmetatable(spWorker) == soup.Worker or getmetatable(spWorker) == soup.EstablishWebSocketConnectionTask or getmetatable(spWorker) == soup.HttpRequestTask or getmetatable(spWorker) == soup.Socket or getmetatable(spWorker) == soup.WebSocketConnection)
			libsoup:call("Scheduler_add", self.addr, spWorker.addr)
		end,
		shouldKeepRunning = function(self)
			return 0 ~= libsoup:call("Scheduler_shouldKeepRunning", self.addr)
		end,
		tick = function(self)
			libsoup:call("Scheduler_tick", self.addr)
		end,
	},
	EstablishWebSocketConnectionTask = {
		__name = "soup.EstablishWebSocketConnectionTask",
		__gc = function(self)
			libsoup:call("endLifetime", self.addr)
		end,
		new = function(uri)
			return initClass(soup.EstablishWebSocketConnectionTask, { addr = libsoup:call("EstablishWebSocketConnectionTask_new", uri) })
		end,
		getSocket = function(self)
			return initClass(soup.WebSocketConnection, { addr = libsoup:call("EstablishWebSocketConnectionTask_getSocket", self.addr) })
		end,
		isWorkDone = function(self)
			return 0 ~= libsoup:call("Worker_isWorkDone", self.addr)
		end,
	},
	HttpRequest = {
		__name = "soup.HttpRequest",
		__gc = function(self)
			libsoup:call("endLifetime", self.addr)
		end,
		new = function(uri)
			return initClass(soup.HttpRequest, { addr = libsoup:call("HttpRequest_new", uri) })
		end,
		setPayload = function(self, data)
			libsoup:call("HttpRequest_setPayload", self.addr, data)
		end,
		getPath = function(self)
			return libsoup:callString("HttpRequest_getPath", self.addr)
		end,
		addHeader = function(self, key, value)
			libsoup:call("MimeMessage_addHeader", self.addr, key, value)
		end,
	},
	HttpRequestTask = {
		__name = "soup.HttpRequestTask",
		__gc = function(self)
			libsoup:call("endLifetime", self.addr)
		end,
		newFromRequest = function(hr)
			assert(getmetatable(hr) == soup.HttpRequest)
			return initClass(soup.HttpRequestTask, { addr = libsoup:call("HttpRequestTask_newFromRequest", hr.addr) })
		end,
		isWorkDone = function(self)
			return 0 ~= libsoup:call("Worker_isWorkDone", self.addr)
		end,
	},
	MimeMessage = {
		__name = "soup.MimeMessage",
		__gc = function(self)
			libsoup:call("endLifetime", self.addr)
		end,
		addHeader = function(self, key, value)
			libsoup:call("MimeMessage_addHeader", self.addr, key, value)
		end,
	},
	PromiseBase = {
		__name = "soup.PromiseBase",
		__gc = function(self)
			libsoup:call("endLifetime", self.addr)
		end,
		isPending = function(self)
			return 0 ~= libsoup:call("PromiseBase_isPending", self.addr)
		end,
	},
	Promise_WebSocketMessage = {
		__name = "soup.Promise_WebSocketMessage",
		__gc = function(self)
			libsoup:call("endLifetime", self.addr)
		end,
		getData = function(self)
			return libsoup:callString("Promise_WebSocketMessage_getData", self.addr)
		end,
		isPending = function(self)
			return 0 ~= libsoup:call("PromiseBase_isPending", self.addr)
		end,
	},
	Scheduler = {
		__name = "soup.Scheduler",
		__gc = function(self)
			libsoup:call("endLifetime", self.addr)
		end,
		new = function()
			return initClass(soup.Scheduler, { addr = libsoup:call("Scheduler_new") })
		end,
		setDontMakeReusableSockets = function(self)
			libsoup:call("Scheduler_setDontMakeReusableSockets", self.addr)
		end,
		add = function(self, spWorker)
			assert(getmetatable(spWorker) == soup.Worker or getmetatable(spWorker) == soup.EstablishWebSocketConnectionTask or getmetatable(spWorker) == soup.HttpRequestTask or getmetatable(spWorker) == soup.Socket or getmetatable(spWorker) == soup.WebSocketConnection)
			libsoup:call("Scheduler_add", self.addr, spWorker.addr)
		end,
		shouldKeepRunning = function(self)
			return 0 ~= libsoup:call("Scheduler_shouldKeepRunning", self.addr)
		end,
		tick = function(self)
			libsoup:call("Scheduler_tick", self.addr)
		end,
	},
	ServerService = {
		__name = "soup.ServerService",
		__gc = function(self)
			libsoup:call("endLifetime", self.addr)
		end,
	},
	ServerWebService = {
		__name = "soup.ServerWebService",
		__gc = function(self)
			libsoup:call("endLifetime", self.addr)
		end,
		new = function()
			return initClass(soup.ServerWebService, { addr = libsoup:call("ServerWebService_new") })
		end,
		hasPendingRequest = function(self)
			return 0 ~= libsoup:call("ServerWebService_hasPendingRequest", self.addr)
		end,
		getPendingRequestPath = function(self)
			return libsoup:callString("ServerWebService_getPendingRequestPath", self.addr)
		end,
		ignoreRequest = function(self)
			libsoup:call("ServerWebService_ignoreRequest", self.addr)
		end,
		replyWithHtml = function(self, html)
			libsoup:call("ServerWebService_replyWithHtml", self.addr, html)
		end,
		replyWith404 = function(self)
			libsoup:call("ServerWebService_replyWith404", self.addr)
		end,
	},
	Socket = {
		__name = "soup.Socket",
		__gc = function(self)
			libsoup:call("endLifetime", self.addr)
		end,
		isWorkDone = function(self)
			return 0 ~= libsoup:call("Worker_isWorkDone", self.addr)
		end,
	},
	Totp = {
		__name = "soup.Totp",
		__gc = function(self)
			libsoup:call("endLifetime", self.addr)
		end,
		new = function(secret)
			assert(getmetatable(secret) == soup.stdstring)
			return initClass(soup.Totp, { addr = libsoup:call("Totp_new", secret.addr) })
		end,
		getQrCodeUri = function(self, label, issuer)
			if issuer == nil then
				issuer = ""
			end
			return libsoup:callString("Totp_getQrCodeUri", self.addr, label, issuer)
		end,
		getValue = function(self)
			return libsoup:call("Totp_getValue", self.addr)
		end,
	},
	WebSocketConnection = {
		__name = "soup.WebSocketConnection",
		__gc = function(self)
			libsoup:call("endLifetime", self.addr)
		end,
		wsSend = function(self, text)
			libsoup:call("WebSocketConnection_wsSend", self.addr, text)
		end,
		wsRecv = function(self)
			return initClass(soup.Promise_WebSocketMessage, { addr = libsoup:call("WebSocketConnection_wsRecv", self.addr) })
		end,
		isWorkDone = function(self)
			return 0 ~= libsoup:call("Worker_isWorkDone", self.addr)
		end,
	},
	Worker = {
		__name = "soup.Worker",
		__gc = function(self)
			libsoup:call("endLifetime", self.addr)
		end,
		isWorkDone = function(self)
			return 0 ~= libsoup:call("Worker_isWorkDone", self.addr)
		end,
	},
}

--[[ An example:
soup.tryCatch(function()
	local secret = soup.Hotp.generateSecret()
	local t = soup.Totp.new(secret)
	print(t:getQrCodeUri("Soup"))
	print(string.format("%06d", t:getValue()))
end) ]]
