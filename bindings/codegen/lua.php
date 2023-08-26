<?php
require "common.php";

echo <<<EOC
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

EOC;

function beginNamespace(ApiNamespace $t)
{
	echo "\t{$t->name} = {\n";
}

function beginClass(ApiClass $t)
{
	beginNamespace($t);

	echo "\t\t__name = \"soup.{$t->name}\",\n";

	echo "\t\t__gc = function(self)\n";
	echo "\t\t\tlibsoup:call(\"endLifetime\", self.addr)\n";
	echo "\t\tend,\n";
}

function funcargs(ApiNamespace $t, ApiFunc $func): string
{
	$str = "";
	if ($func->args)
	{
		$str = ", ";
		if ($t->isStatic($func))
		{
			$str .= $func->args[0]->name;
			if (!apiIsPrimitiveType($func->args[0]->type))
			{
				$str .= ".addr";
			}
		}
		else
		{
			$str .= "self.addr";
		}
		for ($i = 1; $i != count($func->args); ++$i)
		{
			$str .= ", ".$func->args[$i]->name;
			if (!apiIsPrimitiveType($func->args[$i]->type))
			{
				$str .= ".addr";
			}
		}
	}
	return $str;
}

function namespaceFuncs(ApiNamespace $t)
{
	global $apiclasses;
	foreach ($t->methods as $name => $func)
	{
		echo "\t\t$name = function(";
		if ($func->args)
		{
			if ($t->isStatic($func))
			{
				echo $func->args[0]->name;
			}
			else
			{
				echo "self";
			}
			for ($i = 1; $i != count($func->args); ++$i)
			{
				echo ", ".$func->args[$i]->name;
			}
		}
		echo ")\n";
		foreach ($func->args as $i => $arg)
		{
			if ($arg->default !== null)
			{
				echo "\t\t\tif {$arg->name} == nil then\n";
				echo "\t\t\t\t{$arg->name} = {$arg->getDefault()}\n";
				echo "\t\t\tend\n";
			}
			if (!apiIsPrimitiveType($arg->type))
			{
				if ($i != 0 || $t->isStatic($func))
				{
					echo "\t\t\tassert(";
					$first = true;
					foreach ($apiclasses[$arg->type]->getCompatibleTypes() as $ct)
					{
						if ($first)
						{
							$first = false;
						}
						else
						{
							echo " || ";
						}
						echo "getmetatable({$arg->name}) == soup.{$ct->name}";
					}
					echo ")\n";
				}
			}
		}
		if ($func->ret == "void")
		{
			echo "\t\t\tlibsoup:call(\"{$func->name}\"".funcargs($t, $func).")\n";
		}
		else if ($func->ret == "bool")
		{
			// 0 is a truthy value in Lua, so we need to "convert" it
			echo "\t\t\treturn 0 ~= libsoup:call(\"{$func->name}\"".funcargs($t, $func).")\n";
		}
		else if (!apiIsPrimitiveType($func->ret))
		{
			echo "\t\t\treturn initClass(soup.{$func->ret}, { addr = libsoup:call(\"{$func->name}\"".funcargs($t, $func).") })\n";
		}
		else if ($func->ret == "string")
		{
			echo "\t\t\treturn libsoup:callString(\"{$func->name}\"".funcargs($t, $func).")\n";
		}
		else
		{
			echo "\t\t\treturn libsoup:call(\"{$func->name}\"".funcargs($t, $func).")\n";
		}
		echo "\t\tend,\n";
	}
}

function endNamespace(ApiNamespace $t)
{
	echo "\t},\n";
}

foreach ($apinamespaces as $t)
{
	beginNamespace($t);
	namespaceFuncs($t);
	endNamespace($t);
}

foreach ($apiclasses as $t)
{
	beginClass($t);
	foreach ($t->getHierarchy() as $t2)
	{
		namespaceFuncs($t2);
	}
	endNamespace($t);
}

echo "}\n";
