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
	char *cmd, *cmd_token, *paths[1] = {NULL};
	int tokens_count, i;

	while (1)
	{
		/* initialize vars */
		getcwd(path, sizeof(path));
		user = getUser();
		hostname = getHostname();
		tokens_count = 0;
		size = 0;
		input = NULL;
		tokens = malloc(64 * sizeof(char *));
		if (tokens == NULL) /* malloc fail check */
			exit(EXIT_FAILURE);

		/* initialize all tokens in array to null */
		for (i = 0; i < 64; i++)
			tokens[i] = NULL;

		printPrompt(isAtty, user, hostname, path);
		saveInput(isAtty, tokens, &size, &input);
		if (parseInput(input, &tokens, &cmd_token, &tokens_count) == -1)
		{
			if (isAtty)
				continue;
			exit(EXIT_SUCCESS);
		}
		initCmd(&cmd, tokens);
		executeIfValid(isAtty, argv, input, tokens, cmd, cmd_token, paths);
		if (cmd == NULL)
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

	free(user);
	free(hostname);
}

/**
 * saveInput - get & save input
 *
 * @isAtty: isatty() return. if 1 interactive, 0 otherwise
 * @tokens: empty container for tokenized inputs
 * @size: size of input
 * @input: user-input
 */
void saveInput(int isAtty, char **tokens, size_t *size, char **input)
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

		freeAll(tokens, (*input), NULL);
		exit(EXIT_SUCCESS);
	}
}

/**
 * parseInput - parsed the user-input into tokens
 * @input: user-input
 * @tokens: empty token containers
 * @cmd_token: strtok tool
 * @tokens_count: token counter
 *
 * Return: 1 if successful, -1 if failed
 */
int parseInput(char *input, char ***tokens, char **cmd_token,
				int *tokens_count)
{
	(*cmd_token) = strtok(input, " \n\t\r"); /* first token */
	if ((*cmd_token) == NULL) /* blank command - only spaces or newline */
	{
		freeAll((*tokens), input, NULL);
		return (-1); /* go back to start of the loop */
	}

	if (populateTokens(input, tokens, cmd_token, tokens_count) == -1)
		return (-1);

	if ((*tokens) == NULL)
	{
		freeAll((*tokens), /*cmd, */input, NULL);
		return (-1);
	}
	(*tokens)[(*tokens_count)] = NULL;

	return (1);
}

/**
 * populateTokens: populates the tokens with user-inputs
 * @input: user-input
 * @tokens: populates tokens with parsed cmd_token
 * @cmd_token: container of delimited tokens
 * @tokens_count: token counter
 *
 * Return: 1 if successful, -1 if not
 */
int populateTokens(const char *input, char ***tokens, char **cmd_token,
				   int *tokens_count)
{
	while ((*cmd_token) != NULL)
	{
		if ((*tokens_count) >= 64)
			(*tokens) = realloc((*tokens), (*tokens_count) * sizeof(char *));

		if ((*tokens) == NULL)
		{
			freeAll((*tokens), input, NULL);
			return (-1);
		}

		(*tokens)[(*tokens_count)] = strdup((*cmd_token));
		(*cmd_token) = strtok(NULL, " \n\t\r");
		(*tokens_count)++;
	}
	return (1);
}
