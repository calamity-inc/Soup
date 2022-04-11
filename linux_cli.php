<?php
require "linux_config.php";

$clang .= " -fuse-ld=";
if(shell_exec("which lld"))
{
	$clang .= "l";
}
$clang .= "ld -lstdc++ -lstdc++fs -pthread -lresolv -lm";

passthru("$clang \"examples/CLI/cli.cpp\" \"examples/CLI/cli_3d.cpp\" libsoup.a -I soup -o \"soupcli\"");
