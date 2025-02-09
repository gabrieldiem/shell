#ifndef BUILTIN_H
#define BUILTIN_H

#include "defs.h"

extern char prompt[PRMTLEN];

int cd(char *cmd, char *prompt, int *status);

int exit_shell(char *cmd, int *status);

int pwd(char *cmd, int *status);

int history(char *cmd, int *status);

#endif  // BUILTIN_H
