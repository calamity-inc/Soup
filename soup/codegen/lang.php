<?php
function joaat(string $str): int
{
	return hexdec(hash("joaat", $str));
}

$fh = fopen("../loc_data.hpp", "wb");
foreach (scandir("../../lang") as $file)
{
	if (substr($file, -4) == ".txt")
	{
		$code = substr($file, 0, -4);
		$cont = file_get_contents("../../lang/".$file);
		$uncompressed = "";
		foreach (explode("\n", $cont) as $line)
		{
			$line = rtrim($line, "\r");
			if (!empty($line))
			{
				$sep = strpos($line, "=");
				$key = substr($line, 0, $sep);
				$value = substr($line, $sep + 1);
				$uncompressed .= pack("V", joaat($key));
				$uncompressed .= $value;
				$uncompressed .= "\0";
			}
		}
		$bin_str = gzcompress($uncompressed, 9);
		fwrite($fh, "static const char compressed_".$code."[] = { '\\x".join("', '\\x", array_map("dechex", array_map("ord", str_split($bin_str))))."' };\n");
	}
}
