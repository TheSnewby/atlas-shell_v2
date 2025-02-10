#include "main.h"

#include "main.h"

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
int execute_pipe_command(char **command1, char **command2)
{
	int pipefd[2];
	pid_t pid1, pid2;
	int status1, status2; /* Store status of *both* children */

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return -1;
	}

	if (access(command1[0], F_OK) != 0) /* checks if cmd doesn't exist */
		return (127);

	pid1 = fork();
	if (pid1 == -1)
	{
		perror("fork");
		close(pipefd[0]);
		close(pipefd[1]);
		return -1;
	}

	if (pid1 == 0)
	{ /*Child process 1 */
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		char *full_path1 = findPath(command1[0]);
		if (full_path1)
		{
			execve(full_path1, command1, environ);
			free(full_path1);
		}
		fprintf(stderr, "hsh: 1: %s: not found\n", command1[0]);
		exit(127); /* Command not found */
	}

	pid2 = fork();
	if (pid2 == -1)
	{
		perror("fork");
		close(pipefd[0]);
		close(pipefd[1]);
		waitpid(pid1, NULL, 0); /* Cleanup, but don't check status here */
		return -1;
	}

	if (pid2 == 0)
	{ /* Child process 2 */
		close(pipefd[1]);
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);

		char *full_path2 = findPath(command2[0]);
		if (full_path2)
		{
			execve(full_path2, command2, environ);
			free(full_path2);
		}
		fprintf(stderr, "hsh: 1: %s: not found\n", command2[0]);
		exit(127); /* Command not found */
	}

	/* Parent process */
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(pid1, &status1, 0); /* Wait for *both* children, store *both* statuses */
	waitpid(pid2, &status2, 0);

	if (WIFEXITED(status2))
	{
		return WEXITSTATUS(status2); /* Return status of *second* command */
	}
	else
	{
		return -1; /* Error if second command didn't exit normally */
	}
}

/**
 * execute_command - Executes a single command.
 * @args: The arguments of the command.
 * @path_list: Linked list of PATH directories.
 *
 * Return: 0 on success, appropriate error code on failure.
 */
int execute_command(char *commandPath, char **arguments)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return -1;
	}
	else if (pid == 0)
	{
		/* Child process */
		char *paths = getenv("PATH");
		if (paths == NULL)
		{
			paths = "/bin:/usr/bin"; /* Default PATH if none set */
		}

		char *path = strdup(paths);
		char *saveptr = NULL;
		char *dir = strtok_r(path, ":", &saveptr);
		char fullPath[PATH_MAX];

		while (dir != NULL)
		{
			snprintf(fullPath, PATH_MAX, "%s/%s", dir, commandPath);
			execve(fullPath, arguments, environ);
			dir = strtok_r(NULL, ":", &saveptr);
		}

		free(path);
		perror("execve");
		exit(EXIT_FAILURE);
	}
	else
	{
		/* Parent process */
		if (waitpid(pid, &status, 0) == -1)
		{
			perror("waitpid");
			return -1;
		}

		if (WIFEXITED(status))
		{
			return WEXITSTATUS(status);
		}
		return -1;
	}
}
