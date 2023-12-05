<?php
$clang = "clang -std=c++17 -fno-rtti";
$clanglink = $clang." -lstdc++ -lstdc++fs -pthread -lresolv -lm -ldl";
if (PHP_OS_FAMILY != "Darwin")
{
	$clanglink .= " -fuse-ld=lld";
}
