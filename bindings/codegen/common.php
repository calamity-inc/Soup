<?php
$apispec = json_decode(file_get_contents("apispec.json"), true);
$apifuncs = [];
$apiclasses = [];
$apinamespaces = [];

function apiIsPrimitiveType(string $type): bool
{
	return $type == "int" || $type == "string";
}

class ApiFuncArg
{
	public string $type;
	public string $name;
	public $default;

	public function getDefault(): string
	{
		if (gettype($this->default) == "string")
		{
			return '"'.$this->default.'"';
		}
		return $this->default;
	}
}

class ApiFunc
{
	public string $name;
	public string $ret;
	public array $args;

	public function sanityCheck(): void
	{
		global $apiclasses, $apinamespaces;
		if (array_key_exists($this->ret, $apinamespaces))
		{
			echo "{$this->name} returns a namespace.\n";
		}
		foreach ($this->args as $arg)
		{
			if (!apiIsPrimitiveType($arg->type)
				&& !array_key_exists($arg->type, $apiclasses)
				)
			{
				echo "{$this->name}'s {$arg->name} parameter is of a non-primitive & non-class type ({$arg->type}).\n";
			}
		}
	}
}

class ApiNamespace
{
	public string $name;
	public array $methods;

	public static function fromApispec(array $in): array
	{
		global $apifuncs;
		$out = [];
		foreach ($in as $name => $funcs)
		{
			$type = new static();
			$type->name = $name;
			$type->methods = [];
			foreach ($funcs as $fname => $apifunc)
			{
				if (!array_key_exists($apifunc, $apifuncs))
				{
					echo "{$name}'s {$fname} method maps to non-existent function.\n";
					continue;
				}
				$type->methods[$fname] = $apifuncs[$apifunc];
			}
			$type->sanityCheck();
			$out[$name] = $type;
		}
		return $out;
	}

	protected function sanityCheck(): void
	{
	}

	public function isStatic(ApiFunc $f): bool
	{
		return count($f->args) == 0
			|| $f->args[0]->type != $this->name
			;
	}
}

class ApiClass extends ApiNamespace
{
	protected function sanityCheck(): void
	{
		parent::sanityCheck();
		if (array_key_exists("new", $this->methods))
		{
			$func = $this->methods["new"];
			if ($func->ret != $this->name)
			{
				echo "{$this->name}'s {$name} method (which maps to {$func->name}) returns different type: {$func->ret}\n";
			}
		}
	}
}

foreach($apispec["functions"] as $name => $funcspec)
{
	$func = new ApiFunc();
	$func->name = $name;
	$func->ret = $funcspec[0];
	$func->args = [];
	for ($i = 1; $i != count($funcspec); ++$i)
	{
		$arg = new ApiFuncArg();
		$arg->type = $funcspec[$i][0];
		$arg->name = $funcspec[$i][1];
		$arg->default = $funcspec[$i][2] ?? null;
		array_push($func->args, $arg);
	}
	$apifuncs[$name] = $func;
}

$apiclasses = ApiClass::fromApispec($apispec["classes"]);
$apinamespaces = ApiNamespace::fromApispec($apispec["namespaces"]);

foreach ($apifuncs as $func)
{
	$func->sanityCheck();
}
