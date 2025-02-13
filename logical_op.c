#include "main.h"
#include <stdbool.h>

/**
 * execute_command_group - Executes commands separated by semicolons.
 * @command_group: The command line.
 */
void execute_command_group(char *command_group)
{
	char *saveptr2;
	char *token;
	SeparatorType sep_type = SEP_NONE;
	char *delim = "&|";

	for (token = _strtok_r(command_group, delim, &saveptr2);
		 token != NULL;
		 token = _strtok_r(NULL, delim, &saveptr2))
	{
		char **args = parse_command(token);

		if (args == NULL)
		{
			continue; /* Skip to the next token on parse error */
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
		char *next_sep = saveptr2;

		if (next_sep != NULL)
		{
			if (_strncmp(next_sep, "&&", 2) == 0)
			{
				sep_type = SEP_AND;
			}
			else if (_strncmp(next_sep, "||", 2) == 0)
			{
				sep_type = SEP_OR;
			}
			else
			{
				sep_type = SEP_NONE;
			}
		}
		if (sep_type == SEP_AND && cmd_status != 0)
		{
			break;
		}
		else if (sep_type == SEP_OR && cmd_status == 0)
		{
			break;
		}
	}
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
	char *delim = ";";

	for (command_group = _strtok_r(command_line_copy, delim, &saveptr1);
		 command_group != NULL;
		 command_group = _strtok_r(NULL, delim, &saveptr1))
	{
		execute_command_group(command_group);
	}
	free(command_line_copy);
}
