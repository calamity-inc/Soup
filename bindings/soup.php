<?php
$soup = FFI::cdef(
	str_replace("\nSOUP_CEXPORT ", "",
		file_get_contents("soup.h")
	),
	"soup"
);

/* An example:
$soup->beginScope();
	$m = $soup->InquiryLang_execute("base64_encode Hello");
	echo $soup->InquiryLang_formatResultLine($m);
$soup->endScope();
*/

// Note: PHP-FFI doesn't catch C++ exceptions, which is understandable given that it lives in C land, but this does mean the PHP process just dies if Soup throws.
