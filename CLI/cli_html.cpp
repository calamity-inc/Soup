#include "cli.hpp"

#include <lyoDocument.hpp>
#include <string.hpp>
#include <Window.hpp>

using namespace soup;

void cli_html(const std::string& file)
{
	auto doc = lyoDocument::fromMarkup(string::fromFile(file));
	auto w = doc->createWindow("SoupHTML - Blink and you'll miss it!");
	w.runMessageLoop();
}
