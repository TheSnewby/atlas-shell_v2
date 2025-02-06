#include "main.h"
#include <stdbool.h>

/**
 * split_command_line_on_pipe - Splits the command line into two commands
 *                              based on the pipe symbol without using strchr.
 * @line: The command line string.
 * @command1: Pointer to store the first command (before the pipe).
 * @command2: Pointer to store the second command (after the pipe).
 *
 * Return: 0 if a pipe symbol is found and the line is split, -1 otherwise.
 */
int split_command_line_on_pipe(char *line, char **command1, char **command2)
{
	char *pipe_pos = NULL;
	int i = 0;

	/* Find the pipe symbol manually */
	while (line[i] != '\0')
	{
		if (line[i] == '|')
		{
			pipe_pos = &line[i];
			break;
		}
		i++;
	}
	if (pipe_pos == NULL)
	{
		printf("No pipe found in the command\n"); /*debug*/
		return -1; /* No pipe found */
	}

	*pipe_pos = '\0'; /* Split the string at the pipe symbol */
	*command1 = line;
	*command2 = pipe_pos + 1;

	printf("Command1: %s\n", *command1); /* Debug print */
    printf("Command2: %s\n", *command2);

	return 0;
}

/**
 * parse_command - Parses a command string into an array of arguments.
 * @command: The command string.
 *
 * Return: A NULL-terminated array of strings (arguments) or NULL on error.
 */
char **parse_command(char *command)
{
	int bufsize = 64, position = 0;
	char **tokens = malloc(bufsize * sizeof(char *));
	char *token;

	if (!tokens)
	{
		fprintf(stderr, "hsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(command, " \t\r\n\a");
	while (token != NULL)
	{
		tokens[position] = token;
		position++;

		if (position >= bufsize)
		{
			bufsize += 64;
			tokens = realloc(tokens, bufsize * sizeof(char *));
			if (!tokens)
			{
				fprintf(stderr, "hsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, " \t\r\n\a");
	}
	tokens[position] = NULL;
	return tokens;
}

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

		printf("Executing command1: %s\n", command1[0]); /*debug*/
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

			printf("Executing command2: %s\n", command2[0]); /*debug*/
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

/**
 * execute_commands_separated_by_semicolon - Executes commands separated by semicolons.
 * @line: The command line.
 */
void execute_commands_separated_by_semicolon(char *line)
{
	char *command;
	char *saveptr;
	char **args;

	command = strtok_r(line, ";", &saveptr);
	while (command != NULL)
	{
		args = parse_command(command);
		if (args[0] != NULL)
		{
			execute_command(args);
		}
		free(args);
		command = strtok_r(NULL, ";", &saveptr);
	}
}
typedef enum
{
	SEP_SEMICOLON,
	SEP_AND,
	SEP_OR
} SeparatorType;

/**
 * execute_logical_commands - Executes commands separated by logical operators (&&, ||).
 * @line: The command line.
 */
void execute_logical_commands(char *line)
{
	char *command;
	char *saveptr;
	char **args;
	int status = 0;
	SeparatorType sep_type = SEP_SEMICOLON; /* Start with semicolon behavior */
	bool execute_next = true;				/* Flag to determine if the next command should be executed */

	command = strtok_r(line, ";|&", &saveptr);
	while (command != NULL)
	{
		/* Determine the separator type */
		if (strstr(saveptr - 2, "&&") == saveptr - 2)
		{
			sep_type = SEP_AND;
		}
		else if (strstr(saveptr - 2, "||") == saveptr - 2)
		{
			sep_type = SEP_OR;
		}
		else
		{
			sep_type = SEP_SEMICOLON; /* Default to semicolon if no logical operator found */
		}

		if (execute_next)
		{
			args = parse_command(command);
			if (args[0] != NULL)
			{
				status = execute_command(args);
			}
			free(args);
		}

		/* Determine whether to execute the next command based on status and separator type */
		if (sep_type == SEP_AND)
		{
			execute_next = (status == 0); /* Execute if previous command succeeded */
		}
		else if (sep_type == SEP_OR)
		{
			execute_next = (status != 0); /* Execute if previous command failed */
		}
		else
		{
			execute_next = true; /* Always execute for semicolon */
		}

		command = strtok_r(NULL, ";|&", &saveptr);
	}
}
