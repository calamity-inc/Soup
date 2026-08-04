<?php
$max_args = 20;

function call_case(int $args): string
{
	$str = "\t\tcase ".$args.": return reinterpret_cast<uintptr_t(*)(";
	for ($i = 0; $i != $args; ++$i)
	{
		if($i != 0)
		{
			$str .= ", ";
		}
		$str .= "uintptr_t";
	}
	$str .= ")>(func)(";
	for ($i = 0; $i != $args; ++$i)
	{
		if($i != 0)
		{
			$str .= ", ";
		}
		$str .= "args[".$i."]";
	}
	$str .= ");\n";
	return $str;
}

echo "\t\tswitch (nargs)\n";
echo "\t\t{\n";
for ($i = 0; $i != $max_args + 1; ++$i)
{
	echo call_case($i);
}
echo "\t\t}\n";
