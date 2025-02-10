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

	while (1)
	{
		/* initialize vars */
		getcwd(path, sizeof(path));
		user = getUser();
		hostname = getHostname();
		size = 0;
		input = NULL;
		tokens = NULL; /* Initialize tokens to NULL each iteration. */

		printPrompt(isAtty, user, hostname, path);

		if (getline(&input, &size, stdin) == -1)
		{ /* gets input; plus EOF (^D) check */
			if (isAtty)
			{
				printf("\n%sCtrl-D Entered. %s\n", CLR_DEFAULT_BOLD, CLR_YELLOW_BOLD);
				printf("The %sGates Of Shell%s have closed. Goodbye.\n%s",
					   CLR_RED_BOLD, CLR_YELLOW_BOLD, CLR_DEFAULT);
			}
			if (input)
				free(input); /* Free input before exiting */
			safeExit(EXIT_SUCCESS);
		}

		input[_strcspn(input, "\n")] = 0; /* remove trailing newline */

		/* --- Piping Logic (BEFORE built-in/external command handling) --- */
		char *command1, *command2;
		if (split_command_line_on_pipe(input, &command1, &command2) == 0)
		{
			/* Successfully split on a pipe. */
			char **args1 = parse_command(command1);
			char **args2 = parse_command(command2);

			if (args1 && args2)
			{
				execute_pipe_command(args1, args2);
				/* Free the args arrays allocated by parse_command. */
				free(args1);
				free(args2);
			}
			else
			{
				fprintf(stderr, "Failed to parse commands\n");
			}
			free(input);
			continue; /* Go back to the top of the loop. */
		}
		if (_strstr(input, "&&") || _strstr(input, "||") || _strstr(input, ";"))
		{
			execute_logical_commands(input); /* handle logical operators */
			resetAll(tokens, input);
			continue; /* Return to the main loop after handling logical operators */
		}

		tokens = parse_command(input);
		if (tokens == NULL)
		{
			free(input);
			continue; /* Empty command or parse error, go to next iteration. */
		}

		if (input)
			free(input);
		executeIfValid(isAtty, argv, tokens);
		/* --- Cleanup (ALWAYS done after each command) --- */
		resetAll(tokens, NULL);

		//	execute_logical_commands(input);
		//	continue; /* Return to the main loop after handling logical operators */
	//	}
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
	if (isAtty) /* checks interactive mode */
	{
		/* print thing to let us know we're in this shell, not the real one */
		printf("%s[%sGo$H%s]%s | ", CLR_YELLOW_BOLD, CLR_RED_BOLD,
			   CLR_YELLOW_BOLD, CLR_DEFAULT); /*Go$H stands for Gates of Shell*/
		/* prints user@host in green (i.e. julien@ubuntu) */
		printf("%s%s@%s", CLR_GREEN_BOLD, user, hostname);
		/* prints the path in blue */
		printf("%s:%s%s", CLR_DEFAULT_BOLD, CLR_BLUE_BOLD, path);
		/* resets text color and prints '$ ' */
		printf("%s$ ", CLR_DEFAULT);
	}

	free(user);
	free(hostname);
}
