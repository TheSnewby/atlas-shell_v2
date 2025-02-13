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
int split_command_line_on_pipe(char *input, char ***commands, int *num_commands)
{
	char *token;
	int count = 0;
	char *input_copy;

	/* Count the number of pipes */
	for (int i = 0; input[i] != '\0'; i++)
	{
		if (input[i] == '|')
		{
			count++;
		}
	}

	/* Allocate memory for commands array + NULL terminator */
	*commands = malloc(sizeof(char *) * (count + 2));
	if (*commands == NULL)
	{
		perror("malloc");
		return -1;
	}

	input_copy = _strdup(input); /* Duplicate the input string */
	if (input_copy == NULL)
	{
		perror("_strdup");
		free(*commands);
		return -1;
	}
	token = strtok(input_copy, "|"); /* Tokenize based on pipe*/

	int i = 0;
	while (token != NULL)
	{
		(*commands)[i] = _strdup(token); /* Duplicate and store each token */
		if ((*commands)[i] == NULL)
		{
			perror("_strdup");
			/* Free previously allocated memory before returning */
			for (int j = 0; j < i; j++)
			{
				free((*commands)[j]);
			}
			free(*commands);
			free(input_copy);
			return -1;
		}
		i++;
		token = strtok(NULL, "|"); /* get next command */
	}

	(*commands)[i] = NULL; /* Null-terminate array */
	*num_commands = i;	   /* sets number of commands */
	free(input_copy);
	return 0; /* success */
}

/**
 * parse_command - Parses a command string into an array of arguments.
 * @command: The command string.
 *
 * Return: A NULL-terminated array of strings (arguments) or NULL on error.
 */
char **parse_command(char *command)
{
	int bufsize = 64, position = 0, i;
	char **tokens = malloc(bufsize * sizeof(char *));
	char *token;

	for (i = 0; i < bufsize; i++)
		tokens[i] = NULL;
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
	/* printf("%s\n", tokens[0]); */
	return tokens;
}

// /**
//  * trim_whitespace - Removes leading and trailing whitespace from a string.
//  * @str: The string to trim.
//  *
//  * Return: A pointer to the trimmed string (within the original string).
//  */
// char *trim_whitespace(char *str)
// {
// 	char *end;

// 	/* Trim leading space */
// 	while (isspace((unsigned char)*str))
// 	{
// 		str++;
// 	}

// 	if (*str == 0)
// 	{ /* All spaces? */
// 		return str;
// 	}

// 	/* Trim trailing space */
// 	end = str + strlen(str) - 1;
// 	while (end > str && isspace((unsigned char)*end))
// 	{
// 		end--;
// 	}

// 	/* Write new null terminator character */
// 	end[1] = '\0';

// 	return str;
// }
