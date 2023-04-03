-- See also: https://github.com/calamity-inc/Soup-Lua-Bindings

local ffi = require("luaffi") -- https://github.com/calamity-inc/luaffi
local libsoup = ffi.open("soup")

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
	stdstring = {
		__name = "soup.stdstring",
		__gc = function(self)
			libsoup:call("endLifetime", self.addr)
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
}

--[[ An example:
soup.tryCatch(function()
	local secret = soup.Hotp.generateSecret()
	local t = soup.Totp.new(secret)
	print(t:getQrCodeUri("Soup"))
	print(string.format("%06d", t:getValue()))
end) ]]
