<?php
$max_args = 20;

function call_case(int $args): string
{
	$str = "case ".$args.": return reinterpret_cast<uintptr_t(*)(";
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
	$str .= ");";
	return $str;
}

$fh = fopen("ffi.cpp", "w");
fwrite($fh, "switch (nargs)\n");
fwrite($fh, "{\n");
for ($i = 0; $i != $max_args + 1; ++$i)
{
	fwrite($fh, call_case($i)."\n");
}
fwrite($fh, "}\n");
