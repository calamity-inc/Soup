<?php
$clang = "clang -std=c++17 -fno-rtti";
$clanglink = $clang." -fuse-ld=lld -lstdc++ -lstdc++fs -pthread -lresolv -lm -ldl";
