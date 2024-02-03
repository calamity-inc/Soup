<?php
require "build_common.php";

$cmd = "$clanglink \"CLI/cli.cpp\" \"CLI/cli_3d.cpp\" \"CLI/cli_chatgpt.cpp\" \"CLI/cli_dig.cpp\" \"CLI/cli_datareflection.cpp\" \"CLI/cli_dnsserver.cpp\" \"CLI/cli_dvd.cpp\" \"CLI/cli_ircserver.cpp\" \"CLI/cli_keyboard.cpp\" \"CLI/cli_maze.cpp\" \"CLI/cli_mesh.cpp\" \"CLI/cli_midi.cpp\" \"CLI/cli_morse.cpp\" \"CLI/cli_mouse.cpp\" \"CLI/cli_repl.cpp\" \"CLI/cli_snake.cpp\" \"CLI/cli_test.cpp\" \"CLI/cli_websrv.cpp\" ";
if (defined("PHP_WINDOWS_VERSION_MAJOR"))
{
	$cmd .= "soup.lib -I soup -o \"CLI/soup.exe\"";
}
else
{
	$cmd .= "libsoup.a -I soup -o \"CLI/soup\"";
}
passthru($cmd);
