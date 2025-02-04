// parser.c
#include "main.h"

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
		return -1; /* No pipe found */
	}

	*pipe_pos = '\0'; /* Split the string at the pipe symbol */
	*command1 = line;
	*command2 = pipe_pos + 1;

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