<?php
// Config
$clang = "clang -Ofast -std=c++17 -flto -fno-rtti -fno-exceptions";

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
foreach(scandir("src") as $file)
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
	passthru("$clang -c src/$file.cpp -o bin/int/$file.o");
	array_push($objects, escapeshellarg("bin/int/$file.o"));
}

echo "Bundling static lib...\n";
passthru("ar rc libsoup.a ".join(" ", $objects));
