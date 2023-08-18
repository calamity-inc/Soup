<?php
require "linux_config.php";

passthru("$clanglink \"CLI/cli.cpp\" \"CLI/cli_3d.cpp\" \"CLI/cli_chatgpt.cpp\" \"CLI/cli_dig.cpp\" \"CLI/cli_datareflection.cpp\" \"CLI/cli_dnsserver.cpp\" \"CLI/cli_dvd.cpp\" \"CLI/cli_ircserver.cpp\" \"CLI/cli_maze.cpp\" \"CLI/cli_morse.cpp\" \"CLI/cli_repl.cpp\" \"CLI/cli_snake.cpp\" \"CLI/cli_test.cpp\" \"CLI/cli_websrv.cpp\" libsoup.a -I soup -o \"soupcli\"");
