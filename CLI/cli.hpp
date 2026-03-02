#pragma once

#include <string>

#include "base.hpp"

#if !SOUP_WASM
void cli_3d();
#endif
void cli_bench();
int cli_cat2json(size_t argc, std::string* argv);
#if !SOUP_WASM || SOUP_EMSCRIPTEN
void cli_chatgpt(size_t argc, std::string* argv);
#endif
void cli_datareflection();
#if !SOUP_WASM
void cli_dig(size_t argc, std::string* argv);
void cli_dnsserver(size_t argc, std::string* argv);
void cli_dvd();
void cli_html(const std::string& file);
int cli_ircserver();
#endif
int cli_json2bin(size_t argc, std::string* argv);
#if !SOUP_WASM
void cli_keyboard();
void cli_maze();
#endif
#if !SOUP_WASM || SOUP_EMSCRIPTEN
int cli_mesh(size_t argc, std::string* argv);
#endif
void cli_midi(size_t argc, std::string* argv);
void cli_morse(size_t argc, std::string* argv);
#if !SOUP_WASM
void cli_mouse();
#endif
#if !SOUP_WASM || SOUP_EMSCRIPTEN
void cli_repl();
#endif
#if !SOUP_WASM
void cli_snake();
#endif
#if SOUP_EXCEPTIONS
void cli_test();
#endif
int cli_wast(const std::string& file);
#if !SOUP_WASM || SOUP_EMSCRIPTEN
int cli_websrv(const std::string& dir);
#endif
