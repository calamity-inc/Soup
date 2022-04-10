<?php
require "linux_config.php";

$clang .= " -fuse-ld=";
if(shell_exec("which lld"))
{
	$clang .= "l";
}
$clang .= "ld -lstdc++ -lstdc++fs -pthread -lresolv";

passthru("$clang \"examples/{$argv[1]}/{$argv[1]}.cpp\" libsoup.a -I soup -o \"{$argv[1]}\"");
