<?php
$soup = FFI::cdef(
	str_replace("\nSOUP_CEXPORT ", "",
		file_get_contents("soup.h")
	),
	"soup"
);

function soup_tryCatch($f)
{
	global $soup;
	$ex = $soup->tryCatch($f);
	if ($ex !== NULL)
	{
		throw new Exception($ex);
	}
}

function soup_scope($f)
{
	global $soup;
	$soup->beginScope();
	try
	{
		soup_tryCatch($f);
	}
	catch(Exception $ex)
	{
		$soup->endScope();
		throw $ex;
	}
	$soup->endScope();
}

/* An example:
soup_scope(function()
{
	global $soup;
	$m = $soup->InquiryLang_execute("base64_encode Hello");
	echo $soup->InquiryLang_formatResultLine($m);
});
*/
