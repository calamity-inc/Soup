#include "cli.hpp"

#include <lyoDocument.hpp>
#include <lyoFlatDocument.hpp>
#include <string.hpp>
#include <Window.hpp>

using namespace soup;

static UniquePtr<lyoDocument> doc;
static lyoFlatDocument flatdoc;

void cli_html(const char* file)
{
	doc = lyoDocument::fromMarkup(string::fromFile(file));
	auto w = doc->createWindow("SoupHTML - Blink and you'll miss it!");
	w.runMessageLoop();
}
