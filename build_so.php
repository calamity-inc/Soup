<?php
require "build_common.php";
$clang .= " -D SOUP_STANDALONE";

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
	passthru("$clang -fPIC -fvisibility=hidden -c soup/$file.cpp -o bin/int/$file.o");
	array_push($objects, escapeshellarg("bin/int/$file.o"));
}

echo "Linking shared lib...\n";
passthru("$clanglink --shared -o libsoup.so ".join(" ", $objects));
