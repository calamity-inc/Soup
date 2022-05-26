#pragma once

#define INIT_PAD(parent, to) private: char init_pad [(to) - sizeof(parent)]{}; public:
#define PAD(from, to) private: char pad_to_ ## to [to - (from)]{}; public:
