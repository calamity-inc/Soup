<?php
$max_args = 20;

function call_case(string $argsarr, int $args, int $fargs): string
{
	$str = "\t\tcase ".$args.": return reinterpret_cast<uintptr_t(*)(";
	for ($i = 0; $i != $fargs; ++$i)
	{
		if ($i != 0)
		{
			$str .= ", ";
		}
		$str .= "double";
	}
	for ($i = 0; $i != $args; ++$i)
	{
		if ($i != 0 || $fargs)
		{
			$str .= ", ";
		}
		$str .= "uintptr_t";
	}
	$str .= ")>(func)(";
	for ($i = 0; $i != $fargs; ++$i)
	{
		if ($i != 0)
		{
			$str .= ", ";
		}
		$str .= "fargs[".$i."]";
	}
	for ($i = 0; $i != $args; ++$i)
	{
		if ($i != 0 || $fargs)
		{
			$str .= ", ";
		}
		$str .= $argsarr."[".$i."]";
	}
	$str .= ");\n";
	return $str;
}

/*echo "\t\tswitch (nargs)\n";
echo "\t\t{\n";
for ($i = 0; $i != $max_args + 1; ++$i)
{
	echo call_case("args", $i, 0);
}
echo "\t\t}\n";*/

echo "\t\tswitch (npargs)\n";
echo "\t\t{\n";
for ($i = 0; $i != $max_args + 1; ++$i)
{
	echo call_case("pargs", $i, 8);
}
echo "\t\t}\n";
