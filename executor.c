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
	int status;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return -1;
	}

	pid1 = fork();
	if (pid1 == -1)
	{
		perror("fork");
		close(pipefd[0]);
		close(pipefd[1]);
		return -1;
	}

	if (pid1 == 0)
	{									/* Child process (command1) */
		close(pipefd[0]);				/* Close read end */
		dup2(pipefd[1], STDOUT_FILENO); /* Redirect stdout to write end */
		close(pipefd[1]);				/* Close write end after dup2 */

		if (execve(findPath(command1[0]), command1, environ) == -1)
		{
			perror("hsh");
			exit(EXIT_FAILURE); /* Exit child on execve failure */
		}
	}

	/* Parent process */
	pid2 = fork();
	if (pid2 == -1)
	{
		perror("fork");
		close(pipefd[0]);
		close(pipefd[1]);
		waitpid(pid1, NULL, 0); /* Wait for first child, best effort */
		return -1;
	}

	if (pid2 == 0)
	{								   /* Second child process (command2) */
		close(pipefd[1]);			   /* Close write end */
		dup2(pipefd[0], STDIN_FILENO); /* Redirect stdin to read end */
		close(pipefd[0]);			   /* Close read end after dup2 */

		if (execve(findPath(command2[0]), command2, environ) == -1)
		{
			perror("hsh");
			exit(EXIT_FAILURE); /* Exit child on execve failure */
		}
	}

	/* Parent process (continued) */
	close(pipefd[0]); /* Close read end */
	close(pipefd[1]); /* Close write end */

	waitpid(pid1, NULL, 0);	   /* Wait for command1 */
	waitpid(pid2, &status, 0); /* Wait for command2 */

	if (WIFEXITED(status))
	{
		return WEXITSTATUS(status); /* Return exit status of second command */
	}
	else
	{
		return -1; /* Error if no exit status */
	}
}

/**
 * execute_command - Executes a single command.
 * @args: The arguments of the command.
 * @path_list: Linked list of PATH directories.
 *
 * Return: 0 on success, appropriate error code on failure.
 */
int execute_command(char **args)
{
	pid_t pid;
	int status;
	// char *full_path;

	if (args == NULL || args[0] == NULL)
	{
		return (1); // Return a non-zero value for empty command
	}

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return -1;
	}

	if (pid == 0)
	{ /* Child process */
		/* Try execvp first. This handles absolute/relative paths AND PATH lookup */
		execvp(args[0], args);

		/* If execvp fails, it means the command wasn't found (or there was another error) */
		perror("execvp"); /* Use perror to print a more informative error */
		exit(127);		  /* Exit with a "command not found" status */
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
			return -1; /* Indicate an error if the child didn't exit normally */
		}
	}
}
