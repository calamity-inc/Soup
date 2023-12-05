<?php
require "build_config.php";

passthru("$clang -fuse-ld=lld -lstdc++ -lstdc++fs -pthread -lresolv -lm \"examples/{$argv[1]}/{$argv[1]}.cpp\" libsoup.a -I soup -o \"{$argv[1]}\"");
