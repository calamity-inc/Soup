<?php
$clang = "em++ -O3 -std=c++20 -flto -fvisibility=hidden";

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
	echo $file."\n";
	passthru("$clang -c soup/$file.cpp -o bin/int/$file.o");
	array_push($objects, escapeshellarg("bin/int/$file.o"));
}

echo "Linking...\n";
$clang .= " -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=libsoup -s EXPORTED_RUNTIME_METHODS=[\"cwrap\"]";
passthru("$clang -o web/libsoup.js ".join(" ", $objects));

if(is_dir("/var/www/use.soup.do"))
{
	copy("web/libsoup.js", "/var/www/use.soup.do/libsoup.js");
	copy("web/libsoup.wasm", "/var/www/use.soup.do/libsoup.wasm");
}
