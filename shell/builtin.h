#ifndef BUILTIN_H
#define BUILTIN_H

#include "defs.h"

extern char prompt[PRMTLEN];

int cd(char *cmd, char *prompt);

int exit_shell(char *cmd);

int pwd(char *cmd);

int history(char *cmd);

#endif  // BUILTIN_H
