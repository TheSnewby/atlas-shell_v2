#include "main.h"
#include <stdbool.h>

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
			execute_command(args[0], args);
		}
		free(args);
		command = strtok_r(NULL, ";", &saveptr);
	}
}

/**
 * execute_logical_commands - Executes commands separated by logical operators (&&, ||).
 * @line: The command line.
 */
void execute_logical_commands(char *line)
{
	char *command_line_copy = _strdup(line); /* Create a copy*/
	if (command_line_copy == NULL)
	{
		perror("strdup");
		return;
	}

	char *command;
	char *saveptr;
	char **args;
	int status = 0;
	SeparatorType sep_type = SEP_SEMICOLON;
	bool execute_next = true;

	command = strtok_r(command_line_copy, ";|&", &saveptr);
	while (command != NULL)
	{
		/* Determine the separator type */
		char *separator_start = saveptr - 2;
		int end = _strlen(command) - 1;
		while (end >= 0 && (command[end] == ' ' || command[end] == '\t' || command[end] == '\n' || command[end] == '\r'))
		{
			command[end] = '\0';
			end--;
		}
		while (*separator_start == ' ' && separator_start < command_line_copy)
		{
			separator_start++;
		}
		if (separator_start + 1 < saveptr && _strncmp(separator_start, "&&", 2) == 0)
		{
			sep_type = SEP_AND;
		}
		else if (separator_start + 1 < saveptr && _strncmp(separator_start, "||", 2) == 0)
		{
			sep_type = SEP_OR;
		}
		else if (separator_start < saveptr && _strncmp(separator_start, ";", 1) == 0)
		{
			sep_type = SEP_SEMICOLON;
		}
		else
		{
			sep_type = SEP_NONE;
		}

		if (execute_next)
		{
			args = parse_command(command);
			if (args[0] != NULL)
			{
				status = execute_command(args[0], args);
			}
			free(args);
		}

		/* Determine whether to execute the next command based on status and separator type */
		switch (sep_type)
		{
		case SEP_AND:
			execute_next = (status == 0);
			break;
		case SEP_OR:
			execute_next = (status != 0);
			break;
		case SEP_SEMICOLON:
		case SEP_NONE:
		default:
			execute_next = true;
			break;
		}

		command = strtok_r(NULL, ";|&", &saveptr);
	}

	free(command_line_copy);
}
