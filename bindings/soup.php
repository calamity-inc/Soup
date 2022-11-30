<?php
$ffi = FFI::cdef(
	str_replace("\nSOUP_CEXPORT ", "",
		file_get_contents("soup.h")
	),
	"soup"
);

/* An example:
$ffi->beginScope();
	$m = $ffi->InquiryLang_execute("base64_encode Hello");
	echo $ffi->InquiryLang_formatResultLine($m);
$ffi->endScope();
*/

// Note: PHP-FFI doesn't catch C++ exceptions, which is understandable given that it lives in C land, but this does mean the PHP process just dies if Soup throws.
