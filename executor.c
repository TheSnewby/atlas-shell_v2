#include "main.h"
#define FORK_ERROR -1
#define WAITPID_ERROR -2

/**
 * execute_pipe_command - Executes two commands, connecting their
 *                        standard input and output with a pipe.
 * @command1: The first command (left side of pipe).  This is an array
 *             of strings, where command1[0] is the command name,
 *             and subsequent elements are the arguments.  This array
 *             MUST be NULL-terminated.
 * @command2: The second command (right side of pipe).  Same format as
 *             command1.
 *
 * Return: 0 on success, the exit status of the *second* command if
 *         either command fails, or -1 on system call errors (pipe, fork).
 *         Returns 127 if either command is not found.
 */
void execute_pipe_command(char **commands, int num_commands)
{
	if (num_commands <= 0)
	{
		return; /* Nothing to do */
	}

	if (num_commands == 1)
	{
		/* If there's only one command, we don't need pipes where we're going. */
		char **args = parse_command(commands[0]);
		if (args != NULL)
		{
			if (args[0] != NULL)
			{ /* check for empty command */
				char *full_path = findPath(args[0]);
				if (full_path != NULL)
				{
					execute_command(full_path, args);
					free(full_path);
				}
				else
				{ /* command not found */
					fprintf(stderr, "%s: command not found\n", args[0]);
				}
			}
			free(args);
		}
		return;
	}

	/* --- Multiple commands, create pipes --- */

	int pipes[num_commands - 1][2]; /* Array of pipes */
	pid_t pids[num_commands];		/* Array to store child PIDs */

	/* Create all necessary pipes */
	for (int i = 0; i < num_commands - 1; i++)
	{
		if (pipe(pipes[i]) < 0)
		{
			perror("pipe");
			return; /* Handle pipe creation failure */
		}
	}

	/* Fork and execute each command */
	for (int i = 0; i < num_commands; i++)
	{
		pids[i] = fork();
		if (pids[i] < 0)
		{
			perror("fork");
			return; /* Handle fork failure */
		}
		else if (pids[i] == 0)
		{
			/* Child process */

			/* Redirect input (except for the first command) */
			if (i > 0)
			{
				if (dup2(pipes[i - 1][0], STDIN_FILENO) < 0)
				{
					perror("dup2 (stdin)");
					exit(EXIT_FAILURE);
				}
			}

			/* Redirect output (except for the last command) */
			if (i < num_commands - 1)
			{
				if (dup2(pipes[i][1], STDOUT_FILENO) < 0)
				{
					perror("dup2 (stdout)");
					exit(EXIT_FAILURE);
				}
			}

			/* Close all pipe ends in the child */
			for (int j = 0; j < num_commands - 1; j++)
			{
				close(pipes[j][0]);
				close(pipes[j][1]);
			}

			/* Parse the command */
			char **args = parse_command(commands[i]);
			if (args == NULL || args[0] == NULL)
			{
				fprintf(stderr, "Invalid command\n"); /* error message */
				exit(EXIT_FAILURE);
			}
			char *full_path = findPath(args[0]); /* find full path */
			if (full_path == NULL)
			{
				fprintf(stderr, "%s: command not found\n", args[0]);
				exit(127); 
			}
			/* Execute the command */
			execve(full_path, args, environ);
			perror("execve"); /* If execve returns, it failed */
			exit(EXIT_FAILURE);
		}
	}

	/* Parent process: Close all pipe ends */
	for (int i = 0; i < num_commands - 1; i++)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
	}

	/* Wait for all child processes to finish */
	for (int i = 0; i < num_commands; i++)
	{
		waitpid(pids[i], NULL, 0);
	}
}

/**
 * execute_command - Executes a single command.
 * @args: The arguments of the command.
 * @path_list: Linked list of PATH directories.
 *
 * Return: 0 on success, appropriate error code on failure.
 */
int execute_command(const char *commandPath, char **arguments)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return FORK_ERROR;
	}
	else if (pid == 0)
	{
		/* Child process */
		execve(commandPath, arguments, environ);
		perror("execve");	/* execve failed */
		exit(EXIT_FAILURE); /* Exit the child! */
	}
	else
	{
		/* Parent process */
		if (waitpid(pid, &status, 0) == -1)
		{
			perror("waitpid");
			return WAITPID_ERROR;
		}

		if (WIFEXITED(status))
		{
			return WEXITSTATUS(status);
		}
		else if (WIFSIGNALED(status))
		{
			fprintf(stderr, "Command terminated by signal %d\n", WTERMSIG(status));
			return -1; /* Or a specific signal error code */
		}
	}
	return 0; /* Success */
}
