-- This is an example of using an FFI library to interface with Soup's C API.
-- However, there is a more native way of using Soup via Lua: https://github.com/calamity-inc/Soup-Lua-Bindings

local ffi = require("luaffi") -- https://github.com/calamity-inc/luaffi
local soup = ffi.open("soup")

local function soup_tryCatch(f)
	local err = soup:callString("tryCatch", function()
		f()
	end)
	if err ~= nil then
		error(err)
	end
end

local function soup_scope(f)
	soup:call("beginScope")
	local ok, err = pcall(soup_tryCatch, f)
	if not ok then
		soup:call("endScope")
		error(err)
	end
	soup:call("endScope")
end

--[[ An example:
soup_scope(function()
	local m = soup:call("InquiryLang_execute", "base64_encode Hello")
	print(soup:callString("InquiryLang_formatResultLine", m)) -- SGVsbG8=
end) ]]

return soup
