<?php
require "build_common.php";

$clang = "em++ -O3 -std=c++20 -flto -fvisibility=hidden -fwasm-exceptions";

// Setup folders
if(!is_dir("bin"))
{
	mkdir("bin");
}
if(!is_dir("bin/int"))
{
	mkdir("bin/int");
}

// Find work
$files = [];
foreach(scandir("soup") as $file)
{
	if(substr($file, -4) == ".cpp")
	{
		array_push($files, substr($file, 0, -4));
	}
}

echo "Compiling...\n";
$objects = [];
foreach($files as $file)
{
	//if ($file == "soup")
	{
		//echo $file."\n";
		run_command_async("$clang -c soup/$file.cpp -o bin/int/$file.o");
	}
	array_push($objects, escapeshellarg("bin/int/$file.o"));
}
await_commands();

echo "Linking...\n";
$clang .= " -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=libsoup -s EXPORTED_RUNTIME_METHODS=[\"cwrap\"] -s FETCH=1";
//$clang .= " -s ASSERTIONS=1";
//$clang .= " -s LINKABLE=1 -s EXPORT_ALL=1";
//$clang .= " -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=3"; // cursed. for one, only works over HTTPS. I hate modern webdev.
passthru("$clang -o bindings/libsoup.js ".join(" ", $objects));

if(is_dir("/var/www/use.soup.do"))
{
	copy("bindings/libsoup.js", "/var/www/use.soup.do/libsoup.js");
	copy("bindings/libsoup.wasm", "/var/www/use.soup.do/libsoup.wasm");
}
