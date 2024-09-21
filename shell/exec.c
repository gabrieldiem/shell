#include "exec.h"

static const char STR_COMBINE_STREAM_2_INTO_STREAM_1[] = "&1";
static const int LEN_STR_COMBINE_STREAM_2_INTO_STREAM_1 = 3;

// sets "key" with the key part of "arg"
// and null-terminates it
//
// Example:
//  - KEY=value
//  arg = ['K', 'E', 'Y', '=', 'v', 'a', 'l', 'u', 'e', '\0']
//  key = "KEY"
//
static void
get_environ_key(char *arg, char *key)
{
	int i;
	for (i = 0; arg[i] != '='; i++)
		key[i] = arg[i];

	key[i] = END_STRING;
}

// sets "value" with the value part of "arg"
// and null-terminates it
// "idx" should be the index in "arg" where "=" char
// resides
//
// Example:
//  - KEY=value
//  arg = ['K', 'E', 'Y', '=', 'v', 'a', 'l', 'u', 'e', '\0']
//  value = "value"
//
static void
get_environ_value(char *arg, char *value, int idx)
{
	size_t i, j;
	for (i = (idx + 1), j = 0; i < strlen(arg); i++, j++)
		value[j] = arg[i];

	value[j] = END_STRING;
}

// sets the environment variables received
// in the command line
//
// Hints:
// - use 'block_contains()' to
// 	get the index where the '=' is
// - 'get_environ_*()' can be useful here
static void
set_environ_vars(char **eargv, int eargc)
{
	// Your code here
}

// opens the file in which the stdin/stdout/stderr
// flow will be redirected, and returns
// the file descriptor
//
// Find out what permissions it needs.
// Does it have to be closed after the execve(2) call?
//
// Hints:
// - if O_CREAT is used, add S_IWUSR and S_IRUSR
// 	to make it a readable normal file
static int
open_redir_fd(char *file, int flags)
{
	// Your code here

	return -1;
}

static void
run_exec(struct execcmd *exec_cmd)
{
	char *execvp_buff[MAXARGS + 1] = { NULL };

	for (int i = 0; i < exec_cmd->argc; i++) {
		execvp_buff[i] = exec_cmd->argv[i];
	}

	execvp(exec_cmd->argv[0], execvp_buff);
	perror("Error on exec");
}

/*
 * Returns `true` if the stream 2 into stream 1 redirection is present,
 * returns `false` otherwise
 */
static bool
should_combine_stream_2_into_stream_1(struct execcmd *exec_cmd)
{
	return strncmp(exec_cmd->err_file,
	               STR_COMBINE_STREAM_2_INTO_STREAM_1,
	               LEN_STR_COMBINE_STREAM_2_INTO_STREAM_1) == 0;
}

/*
 * Close the current stderr FD and create a new stderr FD that points to
 * the stdout file
 */
static void
combine_stream_2_into_stream_1()
{
	int res = dup2(STDOUT_FILENO, STDERR_FILENO);
	if (res == GENERIC_ERROR_CODE) {
		perror("Error: cannot combine STDERR output into STDOUT");
	}
}

// executes a command - does not return
//
// Hint:
// - check how the 'cmd' structs are defined
// 	in types.h
// - casting could be a good option
void
exec_cmd(struct cmd *cmd)
{
	// To be used in the different cases
	struct execcmd *exec_cmd;
	struct backcmd *b;
	struct execcmd *redir_cmd;
	struct pipecmd *p;

	switch (cmd->type) {
	case EXEC:
		exec_cmd = (struct execcmd *) cmd;
		run_exec(exec_cmd);

		_exit(EXIT_FAILURE);
		break;

	case BACK: {
		// runs a command in background
		//
		// Your code here
		printf("Background process are not yet implemented\n");
		_exit(EXIT_FAILURE);
		break;
	}

	case REDIR: {
		// changes the input/output/stderr flow
		//
		// To check if a redirection has to be performed
		// verify if file name's length (in the execcmd struct)
		// is greater than zero
		//
		redir_cmd = (struct execcmd *) cmd;

		if (should_combine_stream_2_into_stream_1(redir_cmd)) {
			combine_stream_2_into_stream_1();
		}

		run_exec(redir_cmd);

		_exit(EXIT_FAILURE);
		break;
	}

	case PIPE: {
		// pipes two commands
		//
		// Your code here
		printf("Pipes are not yet implemented\n");

		// free the memory allocated
		// for the pipe tree structure
		free_command(parsed_pipe);

		break;
	}
	}
}
