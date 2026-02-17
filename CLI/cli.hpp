#pragma once

#include <string>

void cli_3d();
void cli_bench();
int cli_cat2json(size_t argc, std::string* argv);
void cli_chatgpt(size_t argc, std::string* argv);
void cli_datareflection();
void cli_dig(size_t argc, std::string* argv);
void cli_dnsserver(size_t argc, std::string* argv);
void cli_dvd();
void cli_html(const std::string& file);
int cli_ircserver();
int cli_json2bin(size_t argc, std::string* argv);
void cli_keyboard();
void cli_maze();
int cli_mesh(size_t argc, std::string* argv);
void cli_midi(size_t argc, std::string* argv);
void cli_morse(size_t argc, std::string* argv);
void cli_mouse();
void cli_repl();
void cli_snake();
void cli_test();
int cli_wast(const std::string& file);
int cli_websrv(const std::string& dir);
