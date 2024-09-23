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

static void
verify_redirection_file(int fd)
{
	if (fd == GENERIC_ERROR_CODE) {
		perror("Error: cannot open redirection file");
		// CHEQUEAR
		_exit(EXIT_FAILURE);
	}
}

/*
 * Returns `true` if the output file redirection is present,
 * returns `false` otherwise.
 */
static bool
should_redirect_output_to_file(struct execcmd *exec_cmd)
{
	return strlen(exec_cmd->out_file) > 0;
}

/*
 * Returns `true` if the input file redirection is present,
 * returns `false` otherwise.
 */
static bool
should_redirect_input_from_file(struct execcmd *exec_cmd)
{
	return strlen(exec_cmd->in_file) > 0;
}

/*
 * Returns `true` if the stream 2 into a file redirection is present,
 * returns `false` otherwise
 */
static bool
should_redirect_stream_2_into_file(struct execcmd *exec_cmd)
{
	return strlen(exec_cmd->err_file) > 0;
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
 * Delete the file at `filepath`.
 * If deletion fails, an error is printed.
 */
static void
unlink_file(char *filepath)
{
	int res = unlink(filepath);
	if (res == GENERIC_ERROR_CODE) {
		perror("Error: could not remove the file from the filesystem");
	}
}

/*
 * Opens the file (or creates a new one if it doesn't exist) and writes
 * the STDOUT output on it.
 */
static void
redirect_output_to_file(struct execcmd *exec_cmd)
{
	int fd = open(exec_cmd->out_file,
	              O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC,
	              S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	verify_redirection_file(fd);

	int res = dup2(fd, STDOUT_FILENO);
	if (res == GENERIC_ERROR_CODE) {
		perror("Error: cannot redirect STDOUT output into file");
		res = close(fd);
		if (res == GENERIC_ERROR_CODE) {
			perror("Error while closing file");
		}
		unlink_file(exec_cmd->out_file);
		_exit(EXIT_FAILURE);
	}
}

/*
 * Opens the file and reads the content of it to be used as the STDIN input.
 */
static void
redirect_input_from_file(struct execcmd *exec_cmd)
{
	int fd = open(exec_cmd->in_file,
	              O_RDONLY | O_CLOEXEC,
	              S_IRUSR | S_IRGRP | S_IROTH);
	verify_redirection_file(fd);

	int res = dup2(fd, STDIN_FILENO);
	if (res == GENERIC_ERROR_CODE) {
		perror("Error: cannot redirect STDIN input into file");
		res = close(fd);
		if (res == GENERIC_ERROR_CODE) {
			perror("Error while closing file");
		}
		unlink_file(exec_cmd->out_file);
		_exit(EXIT_FAILURE);
	}
}

/*
 * Opens the file (or creates a new one if it doesn't exist) and writes
 * the STDERR output on it
 */
static void
redirect_stream_2_into_file(struct execcmd *exec_cmd)
{
	int fd = open(exec_cmd->err_file,
	              O_WRONLY | O_CREAT | O_CLOEXEC,
	              S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	verify_redirection_file(fd);

	int res = dup2(fd, STDERR_FILENO);
	if (res == GENERIC_ERROR_CODE) {
		perror("Error: cannot redirect STDERR output into file");
		res = close(fd);
		if (res == GENERIC_ERROR_CODE) {
			perror("Error while closing file");
		}
		unlink_file(exec_cmd->out_file);
		_exit(EXIT_FAILURE);
	}
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
		_exit(EXIT_FAILURE);
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
		// spawns a command
		exec_cmd = (struct execcmd *) cmd;
		run_exec(exec_cmd);

		_exit(EXIT_FAILURE);
		break;

	case BACK: {
		// runs a command in background
		b = (struct backcmd *) cmd;
		run_exec((struct execcmd *) b->c);

		_exit(EXIT_FAILURE);
		break;
	}

	case REDIR: {
		// changes the input/output/stderr flow
		redir_cmd = (struct execcmd *) cmd;

		if (should_redirect_output_to_file(redir_cmd)) {
			redirect_output_to_file(redir_cmd);
		}

		if (should_redirect_input_from_file(redir_cmd)) {
			redirect_input_from_file(redir_cmd);
		}

		if (should_redirect_stream_2_into_file(redir_cmd)) {
			redirect_stream_2_into_file(redir_cmd);
		}

		if (should_combine_stream_2_into_stream_1(redir_cmd)) {
			combine_stream_2_into_stream_1();
		}

		run_exec(redir_cmd);

		_exit(EXIT_FAILURE);
		break;
	}

	case PIPE: {
		p = (struct pipecmd *) cmd;
		int fds[2];
		int r = pipe(fds);
		if (r < 0) {
			_exit(EXIT_FAILURE);
		}

		pid_t pid_left = fork();


		if (pid_left == 0) {
			close(fds[0]);
			int dup_left = dup2(fds[1], STDOUT_FILENO);
			close(fds[1]);
			if (dup_left < 0) {
				_exit(EXIT_FAILURE);
			}
			run_exec((struct execcmd *) p->leftcmd);
			_exit(EXIT_FAILURE);
		}
		pid_t pid_right = fork();
		if (pid_right == 0) {
			close(fds[1]);
			int dup_right = dup2(fds[0], STDIN_FILENO);
			close(fds[0]);
			if (dup_right < 0) {
				_exit(EXIT_FAILURE);
			}
			run_exec((struct execcmd *) p->rightcmd);
			_exit(EXIT_FAILURE);
		} else {
			close(fds[0]);
			close(fds[1]);
			wait(&pid_left);
			wait(&pid_right);
			_exit(0);
			// free_command((struct cmd*) p);
		}


		// free the memory allocated
		// for the pipe tree structure


		break;
	}
	}
}
