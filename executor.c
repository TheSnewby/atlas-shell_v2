#include "main.h"

/**
 * execute_pipe_command - Executes two commands, connecting their
 *                        standard input and output with a pipe.
 * @command1: The first command (to be connected to standard output).
 * @command2: The second command (to be connected to standard input).
 *
 * Return: 0 on success, -1 on error.
 */
int execute_pipe_command(char **command1, char **command2)
{
	int pipefd[2];
	pid_t pid1, pid2;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return -1;
	}

	pid1 = fork();
	if (pid1 == -1)
	{
		perror("fork");
		return -1;
	}

	if (pid1 == 0)
	{									/* Child process (command1) */
		close(pipefd[0]);				/* Close read end */
		dup2(pipefd[1], STDOUT_FILENO); /* Redirect stdout to write end */
		close(pipefd[1]);				/* Close write end after dup2 */

		if (execvp(command1[0], command1) == -1)
		{
			perror("hsh");
			exit(EXIT_FAILURE);
		}
	}
	else
	{ /* Parent process */
		pid2 = fork();
		if (pid2 == -1)
		{
			perror("fork");
			return -1;
		}

		if (pid2 == 0)
		{								   /* Second child process (command2) */
			close(pipefd[1]);			   /* Close write end */
			dup2(pipefd[0], STDIN_FILENO); /* Redirect stdin to read end */
			close(pipefd[0]);			   /* Close read end after dup2 */

			if (execvp(command2[0], command2) == -1)
			{
				perror("hsh");
				exit(EXIT_FAILURE);
			}
		}
		else
		{					  /* Parent process (continued) */
			close(pipefd[0]); /* Close read end */
			close(pipefd[1]); /* Close write end */

			waitpid(pid1, NULL, 0); /* Wait for command1 */
			int status;
			waitpid(pid2, &status, 0); /* Wait for command2 */

			if (WIFEXITED(status))
			{
				if (WEXITSTATUS(status) != 0)
				{
					return WEXITSTATUS(status); /* Return non-zero if either command failed */
				}
			}
			else
			{
				return -1; /* If command didn't exit normally */
			}
		}
	}

	return 0;
}

/**
 * execute_command - Executes a single command.
 * @args: The arguments of the command.
 *
 * Return: 0 on success, -1 on error.
 */
int execute_command(char **args)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return -1;
	}

	if (pid == 0)
	{ /* Child process */
		if (execvp(args[0], args) == -1)
		{
			fprintf(stderr, "hsh: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	else
	{ /* Parent process */
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
		{
			return WEXITSTATUS(status);
		}
		else
		{
			return -1;
		}
	}

	return 0;
}