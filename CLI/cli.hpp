#pragma once

#include <string>

#include "base.hpp"

void cli_3d();
void cli_bench();
int cli_cat2json(size_t argc, std::string* argv);
#if !SOUP_WASM || SOUP_EMSCRIPTEN
void cli_chatgpt(size_t argc, std::string* argv);
#endif
void cli_datareflection();
#if !SOUP_WASM || SOUP_EMSCRIPTEN
void cli_dig(size_t argc, std::string* argv);
void cli_dnsserver(size_t argc, std::string* argv);
#endif
void cli_dvd();
void cli_html(const std::string& file);
#if !SOUP_WASM || SOUP_EMSCRIPTEN
int cli_ircserver();
#endif
int cli_json2bin(size_t argc, std::string* argv);
void cli_keyboard();
void cli_maze();
#if !SOUP_WASM || SOUP_EMSCRIPTEN
int cli_mesh(size_t argc, std::string* argv);
#endif
void cli_midi(size_t argc, std::string* argv);
void cli_morse(size_t argc, std::string* argv);
void cli_mouse();
#if !SOUP_WASM || SOUP_EMSCRIPTEN
void cli_repl();
#endif
void cli_snake();
void cli_test();
int cli_wast(const std::string& file);
#if !SOUP_WASM || SOUP_EMSCRIPTEN
int cli_websrv(const std::string& dir);
#endif
