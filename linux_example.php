<?php
require "linux_config.php";

$clang .= " -fuse-ld=";
if(shell_exec("which lld"))
{
	$clang .= "l";
}
$clang .= "ld -lstdc++ -pthread";

passthru("$clang \"examples/{$argv[1]}/{$argv[1]}.cpp\" libsoup.a -I src -o \"{$argv[1]}\"");
