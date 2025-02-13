#include "main.h"
#include "colors.h"
/**
 * shellLoop - main loop for input/output.
 *
 * @isAtty: is interactive mode
 * @argv: args passed into main()
 */
void shellLoop(int isAtty, char *argv[])
{
	size_t size;
	char *user, *hostname, path[PATH_MAX], *input, **tokens = NULL;
	int custom_cmd_rtn;
	int num_commands;
	char **commands = NULL;

	while (1)
	{
		/* Initialize variables */
		getcwd(path, sizeof(path));
		user = getUser();
		hostname = getHostname();
		size = 0;
		input = NULL;
		tokens = NULL;

		printPrompt(isAtty, user, hostname, path);
		free(user);
		free(hostname);

		if (getline(&input, &size, stdin) == -1)
		{
			if (isAtty)
			{
				printf("\n%sCtrl-D Entered. %s\n", CLR_DEFAULT_BOLD, CLR_YELLOW_BOLD);
				printf("The %sGates Of Shell%s have closed. Goodbye.\n%s",
					   CLR_RED_BOLD, CLR_YELLOW_BOLD, CLR_DEFAULT);
			}
			free(input);
			safeExit(EXIT_SUCCESS);
		}

		input[_strcspn(input, "\n")] = 0; /* Remove trailing newline */

		/* Piping Logic */
		if (_strchr(input, '|'))
		{ /* Check if there's a pipe in the command */
			if (split_command_line_on_pipe(input, &commands, &num_commands) == 0)
			{
				execute_pipe_command(commands, num_commands);
				for (int i = 0; i < num_commands; i++)
				{
					free(commands[i]);
				}
				free(commands);
				free(input);
				continue;
			}
			else
			{
				fprintf(stderr, "Failed to split commands\n");
			}
		}

		/* Logical Operators */
		if (_strstr(input, "&&") || _strstr(input, "||") || _strstr(input, ";"))
		{
			execute_logical_commands(input);
			free(input);
			continue;
		}

		/* Parse and Execute Single Command */
		tokens = parse_command(input);
		if (tokens == NULL)
		{
			free(input);
			continue;
		}
		// --- Built-in Command Handling ---
		custom_cmd_rtn = customCmd(tokens, isAtty, input);
		if (custom_cmd_rtn == 1)
		{
			// Built-in was handled.  Free resources and continue.
			free(tokens);
			free(input);
			continue;
		}

		executeIfValid(isAtty, argv, tokens, input);
		/* --- Cleanup (ALWAYS done after each command) --- */
		free(tokens);
		free(input);
	}
}
/**
 * printPrompt - prints prompt in color ("[Go$H] | user@hostname:path$ ")
 *
 * @isAtty: is interactive mode
 * @user: environment variable for user's username
 * @hostname: environment variable for user's hostname or device name.
 * @path: current working directory
 */
void printPrompt(int isAtty, char *user, char *hostname, char *path)
{
	if (isAtty)
	{
		printf("%s[%sGo$H%s]%s | ", CLR_YELLOW_BOLD, CLR_RED_BOLD,
			   CLR_YELLOW_BOLD, CLR_DEFAULT);
		printf("%s%s@%s", CLR_GREEN_BOLD, user, hostname);
		printf("%s:%s%s", CLR_DEFAULT_BOLD, CLR_BLUE_BOLD, path);
		printf("%s$ ", CLR_DEFAULT);
	}
}
