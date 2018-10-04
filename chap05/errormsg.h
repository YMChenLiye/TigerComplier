#pragma once

#include <stdbool.h>
#include "util.h"

extern bool EM_anyErrors;
extern bool anyErrors;

void EM_newline(void);

extern int EM_tokPos;

void EM_error(int, string, ...);
void EM_impossible(string, ...);
void EM_reset(string filename);
