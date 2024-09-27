#include "defs.h"
#include "types.h"
#include "readline.h"
#include "runcmd.h"
#include "updateprompt.h"

char prompt[PRMTLEN] = { 0 };

/*
 * Frees the alternative stack memory.
 */
static void
free_alternative_stack(stack_t *alternative_stack)
{
	if (alternative_stack->ss_sp != NULL) {
		free(alternative_stack->ss_sp);
		alternative_stack->ss_sp = NULL;
	}
}

/*
 * Custom handler for SIGCHLD signal. Waits only for child processes from same
 * process group, non-blocking.
 */
static void
sigchild_handler(int /*signum*/, siginfo_t *signal_info, void * /*ucontext_t*/)
{
	int status = 0;
	int child_pid = (int) signal_info->si_pid;
	int child_gpid = (int) getpgid(child_pid);
	bool is_background_task = child_pid != child_gpid;

	if (is_background_task) {
		int res = (int) waitpid(child_pid, &status, WNOHANG);
		if (res != GENERIC_ERROR_CODE) {
			print_status_info_from_pid(child_pid, status);
		}
	}
}

/*
 * Initializes the signal handler for SIGCHLD to custom handler.
 */
static void
initialize_sigchild(stack_t *alternative_stack)
{
	struct sigaction signal_action;

	signal_action.sa_flags = SA_SIGINFO | SA_RESTART;
	signal_action.sa_sigaction = sigchild_handler;
	sigemptyset(&signal_action.sa_mask);

	int res = sigaction(SIGCHLD, &signal_action, NULL);
	if (res == GENERIC_ERROR_CODE) {
		perror("Error while setting up sigaction");
		free_alternative_stack(alternative_stack);
		exit(EXIT_FAILURE);
	}

	if (sigaltstack(alternative_stack, NULL) == GENERIC_ERROR_CODE) {
		perror("Error while installing alternative stack");
		free_alternative_stack(alternative_stack);
		exit(EXIT_FAILURE);
	};
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
init_shell(stack_t *alternative_stack)
{
	char buf[BUFLEN] = { 0 };
	char *home = getenv(HOME_ENV_VAR_KEY);

	if (chdir(home) < 0) {
		snprintf(buf, sizeof buf, "cannot cd to %s ", home);
		perror(buf);
	} else {
		update_prompt(prompt, home);
	}

	setpgid(USE_PID_OF_THIS_PROCESS, SET_GPID_SAME_AS_PID_OF_THIS_PROCESS);
	initialize_sigchild(alternative_stack);
}

int
main(void)
{
	void *alt_stack_ptr = malloc(SIGSTKSZ);

	if (alt_stack_ptr == NULL) {
		perror("Error while allocating memory for signal alternative "
		       "stack");
		exit(EXIT_FAILURE);
	}

	stack_t alternative_stack;
	alternative_stack.ss_sp = alt_stack_ptr;
	alternative_stack.ss_size = SIGSTKSZ;
	alternative_stack.ss_flags = SS_ONSTACK;

	init_shell(&alternative_stack);

	run_shell(&alternative_stack);

	free_alternative_stack(&alternative_stack);
	return 0;
}
