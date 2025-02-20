#include "main.h"
#include <stdbool.h>

/**
 * parse_and_execute_logical - Parses and executes a command group with && and ||.
 * @command_group: A string containing commands potentially joined by && or ||.
 *
 * Return: The exit status of the last command executed.
 */
int parse_and_execute_logical(char *command_group)
{
	char *saveptr;
	char *token;
	int prev_status = 0; /* Status of the previous command */
	SeparatorType sep_type = SEP_NONE;

	char *command_group_copy = _strdup(command_group);
	if (command_group_copy == NULL)
	{
		perror("_strdup");
		return -1;
	}

	token = _strtok_r(command_group_copy, "&|", &saveptr);

	while (token != NULL)
	{
		char **args = parse_command(token);
		if (args == NULL)
		{
			free(command_group_copy);
			return -1; /*  parsing error */
		}

		int cmd_status = 0;
		if (args[0] != NULL)
		{
			char *full_path = findPath(args[0]);
			if (full_path != NULL)
			{
				cmd_status = execute_command(full_path, args);
				free(full_path);
			}
			else
			{
				fprintf(stderr, "Command not found: %s\n", args[0]);
				cmd_status = 127;
			}
		}
		free(args);
		/* Determine the next separator type */
		if (saveptr != NULL)
		{
			if (_strncmp(saveptr, "&&", 2) == 0)
			{
				sep_type = SEP_AND;
				saveptr += 2; /* Move past the && */
			}
			else if (_strncmp(saveptr, "||", 2) == 0)
			{
				sep_type = SEP_OR;
				saveptr += 2; /* Move past the || */
			}
			else
			{
				sep_type = SEP_NONE; /* Important: Reset if no operator */
			}
		}

		/* Handle short-circuiting */
		if (sep_type == SEP_AND && cmd_status != 0)
		{
			free(command_group_copy);
			return cmd_status; /* Stop on failure with && */
		}
		if (sep_type == SEP_OR && cmd_status == 0)
		{
			free(command_group_copy);
			return cmd_status; /* Stop on success with || */
		}

		prev_status = cmd_status;
		token = _strtok_r(NULL, "&|", &saveptr); /* Get next command */
	}
	free(command_group_copy);

	return prev_status; /* Return the status of the last command run */
}

/**
 * execute_logical_commands - Executes commands separated by logical operators
 *                            (&&, ||) and semicolons (;).
 * @input: The command line input.
 */
void execute_logical_commands(char *input)
{
	char *command_line_copy = _strdup(input);
	if (command_line_copy == NULL)
	{
		perror("_strdup");
		return;
	}

	char *saveptr1;
	char *command_group;

	/* First, split by semicolons */
	for (command_group = _strtok_r(command_line_copy, ";", &saveptr1);
		 command_group != NULL;
		 command_group = _strtok_r(NULL, ";", &saveptr1))
	{
		/* Now, process EACH semicolon-separated group for && and || */
		parse_and_execute_logical(command_group);
	}

	free(command_line_copy);
}
