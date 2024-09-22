#include "defs.h"
#include "types.h"
#include "readline.h"
#include "runcmd.h"
#include "updateprompt.h"

char prompt[PRMTLEN] = { 0 };


static void
sigchild_handler(int signum) {
    pid_t pid; int status;

    pid = waitpid(0, &status, WNOHANG);

	if (pid > 0) {	
		printf("==> Process %d exited with status %d.\n", pid, status);
	}

}

static void
initialize_sigchild() {
	struct sigaction sa;

	sa.sa_handler = sigchild_handler;
	sa.sa_flags = SA_RESTART;

	// TODO: add alternative stack

	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction failed");
		exit(EXIT_FAILURE);
	}
}


// runs a shell command
static void
run_shell()
{
	char *cmd;

	while ((cmd = read_line(prompt)) != NULL)
		if (run_cmd(cmd, prompt) == EXIT_SHELL)
			return;
}

// initializes the shell
// with the "HOME" directory
static void
init_shell()
{
	char buf[BUFLEN] = { 0 };
	char *home = getenv(HOME_ENV_VAR_KEY);

	if (chdir(home) < 0) {
		snprintf(buf, sizeof buf, "cannot cd to %s ", home);
		perror(buf);
	} else {
		update_prompt(prompt, home);
	}
	setpgid(0, 0);
	initialize_sigchild();
}

int
main(void)
{
	init_shell();

	run_shell();

	return 0;
}
