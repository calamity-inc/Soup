<?php
require "build_config.php";

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

echo "Bundling static lib...\n";
$archiver = "ar";
if (defined("PHP_WINDOWS_VERSION_MAJOR"))
{
	$archiver = "llvm-ar";
}
passthru("$archiver rc libsoup.a ".join(" ", $objects));
