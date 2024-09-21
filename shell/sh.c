#include "defs.h"
#include "types.h"
#include "readline.h"
#include "runcmd.h"
#include "updateprompt.h"

char prompt[PRMTLEN] = { 0 };

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
}

int
main(void)
{
	init_shell();

	run_shell();

	return 0;
}
