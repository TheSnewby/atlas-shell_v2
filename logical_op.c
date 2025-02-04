// logical_operators.c
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
			execute_command(args);
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
		if (saveptr - command >= 2)
		{
			if (strncmp(saveptr - 2, "&&", 2) == 0)
			{
				sep_type = SEP_AND;
			}
			else if (strncmp(saveptr - 2, "||", 2) == 0)
			{
				sep_type = SEP_OR;
			}
			else if (strncmp(saveptr - 1, ";", 1) == 0)
			{
				sep_type = SEP_SEMICOLON;
			}
		}
		else if (saveptr - command >= 1)
		{
			if (strncmp(saveptr - 1, ";", 1) == 0)
			{
				sep_type = SEP_SEMICOLON;
			}
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
