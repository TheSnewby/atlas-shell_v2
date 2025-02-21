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
	char *command_group_copy;

	command_group_copy = _strdup(command_group);
	if (command_group_copy == NULL)
	{
		perror("_strdup");
		return -1;
	}

	token = _strtok_r(command_group_copy, "&|;", &saveptr);
	while (token != NULL)
	{
		// Trim leading and trailing whitespace from the token
		while (isspace((unsigned char)*token))
			token++;
		char *end = token + _strlen(token) - 1;
		while (end > token && isspace((unsigned char)*end))
			end--;
		*(end + 1) = '\0';

		if (_strlen(token) > 0)
		{ // Check if token is not empty after trimming

			char **args = parse_command(token);
			if (args == NULL)
			{
				free(command_group_copy);
				return -1; // Handle parsing error
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

			// Determine the NEXT separator type *before* getting the next token
			if (saveptr != NULL && *saveptr != '\0')
			{

				if (_strncmp(saveptr, "&&", 2) == 0)
				{
					sep_type = SEP_AND;
					saveptr += 2; // Move past
				}
				else if (_strncmp(saveptr, "||", 2) == 0)
				{
					sep_type = SEP_OR;
					saveptr += 2; // Move past
				}
				else if (*saveptr == ';')
				{
					sep_type = SEP_SEMI;
					saveptr += 1; // Move past
				}
				else
				{
					sep_type = SEP_NONE;
				}
			}
			else
			{
				sep_type = SEP_NONE; // No more separators
			}

			// Handle short circuit.
			if (sep_type == SEP_AND && cmd_status != 0)
			{
				free(command_group_copy);
				return cmd_status; // Stop
			}
			if (sep_type == SEP_OR && cmd_status == 0)
			{
				free(command_group_copy);
				return cmd_status; // Stop
			}

			prev_status = cmd_status;
		}
		// Get the next token *after* processing the separator
		token = _strtok_r(NULL, "&|;", &saveptr);
	}

	free(command_group_copy);
	return prev_status;
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
