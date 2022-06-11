<?php
require "linux_config.php";

$clang .= " -fuse-ld=lld -lstdc++ -lstdc++fs -pthread -lresolv -lm";

passthru("$clang \"CLI/cli.cpp\" \"CLI/cli_3d.cpp\" \"CLI/cli_dvd.cpp\" \"CLI/cli_repl.cpp\" \"CLI/cli_snake.cpp\" libsoup.a -I soup -o \"soupcli\"");
