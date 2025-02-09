#include "main.h"
#include "colors.h"

/*
 * shellLoop variables descriptions - because we can't put them on
 * the line they are declared due to betty forcing us to put multiple
 * on each line to shorten the function below 40 lines
 *
 * @size: size for getline()
 * @user: current user name
 * @hostname: host name or device name
 * @path: current working directory
 * @input: user input
 * @tokens: array of strings of tokenized user inputs deliminated by spaces
 * @cmd: user-inputed command with possible path prefixed
 * @cmd_token: tool for strtok
 * @paths: array of strings of env paths
 * @custom_cmd_rtn: return value of customCmd()
 * @run_cmd_rtn: return value of runCommand()
 * @tokens_count: number of tokens while initializing the tokens
 * @i: iterator variable for a for loop somewhere
 */

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

		printPrompt(isAtty, user, hostname, path);
		if (getline(&input, &size, stdin) == -1)
		{ /* gets input; plus EOF (^D) check */
			if (isAtty)
			{
				printf("\n%sCtrl-D Entered. %s\n",
					   CLR_DEFAULT_BOLD, CLR_YELLOW_BOLD);
				printf("The %sGates Of Shell%s have closed.",
					   CLR_RED_BOLD, CLR_YELLOW_BOLD);
				printf("Goodbye.\n%s\n", CLR_DEFAULT);
			}
			free(input);
			safeExit(EXIT_SUCCESS);
		}

		input[strcspn(input, "\n")] = 0; /* remove trailing newline */

		tokens = parse_command(input); /* tokenize input */
		if (tokens == NULL)
		{
			free(input);
			continue; /* Go back to start of the loop */
		}
		if (_strstr(input, "&&") || _strstr(input, "||") || _strstr(input, ";"))
		{
			execute_logical_commands(input);
			free(tokens);
			free(input);
			continue; /* Return to the main loop after handling logical operators */
		}

		executeIfValid(isAtty, argv, input, tokens); /*No more paths*/
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
}

/**
 * saveInput - get & save input
 *
 * @isAtty: isatty() return. if 1 interactive, 0 otherwsie
 * @tokens: empty container for tokenized inputs
 * @size: size of input
 * @input: user-input
 */
void saveInput(int isAtty, size_t *size, char **input)
{
	if (getline(input, size, stdin) == -1) /* gets input; plus EOF (^D) check */
	{
		if (isAtty)
		{
			printf("\n%sCtrl-D Entered. %s\n",
				   CLR_DEFAULT_BOLD, CLR_YELLOW_BOLD);
			printf("The %sGates Of Shell%s have closed.",
				   CLR_RED_BOLD, CLR_YELLOW_BOLD);
			printf("Goodbye.\n%s\n", CLR_DEFAULT);
		}

		free(*input);
		safeExit(EXIT_SUCCESS);
	}
}
