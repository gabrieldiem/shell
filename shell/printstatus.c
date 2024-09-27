#include <unistd.h>

#include "printstatus.h"

// prints information of process' status
void
print_status_info(struct cmd *cmd)
{
	const char *action;

	if (strlen(cmd->scmd) == 0 || cmd->type == PIPE)
		return;

	if (WIFEXITED(status)) {
		action = "exited";
		status = WEXITSTATUS(status);
	} else if (WIFSIGNALED(status)) {
		action = "killed";
		status = -WTERMSIG(status);
	} else if (WTERMSIG(status)) {
		action = "stopped";
		status = -WSTOPSIG(status);
	} else {
		return;
	}

#ifndef SHELL_NO_INTERACTIVE
	if (isatty(1)) {
		fprintf(stdout,
		        "%s	Program: [%s] %s, status: %d %s\n",
		        COLOR_BLUE,
		        cmd->scmd,
		        action,
		        status,
		        COLOR_RESET);
	}
#endif
}

// prints information of process' status from a PID
void
print_status_info_from_pid(int pid, int _status)
{
	const char *action;

	if (WIFEXITED(_status)) {
		action = "exited";
		_status = WEXITSTATUS(_status);
	} else if (WIFSIGNALED(_status)) {
		action = "killed";
		_status = -WTERMSIG(_status);
	} else if (WTERMSIG(_status)) {
		action = "stopped";
		_status = -WSTOPSIG(_status);
	} else {
		action = "exited";
	}

#ifndef SHELL_NO_INTERACTIVE
	if (isatty(1)) {
		fprintf(stdout,
		        "%s	Program with PID [%d] %s, status: "
		        "%d %s\n",
		        COLOR_BLUE,
		        pid,
		        action,
		        _status,
		        COLOR_RESET);
	}
#endif
}

// prints info when a background process is spawned
void
print_back_info(struct cmd *back)
{
#ifndef SHELL_NO_INTERACTIVE
	if (isatty(1)) {
		fprintf(stdout, "%s  [PID=%d] %s\n", COLOR_BLUE, back->pid, COLOR_RESET);
	}
#endif
}
