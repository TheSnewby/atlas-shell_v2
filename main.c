#include "main.h"

/**
 * main - starts the program and the loop
 * @argc: number of arguments
 * @argv: array of arguments
 *
 * Return: 0
 */
int main(int argc, char *argv[])
{
	int isInteractive = isatty(STDIN_FILENO);

	/* ------------------- On entry - one time execution ------------------- */
	(void)argc;
	if (isInteractive)
		printf("%sWelcome to the %sGates Of Shell%s. Type 'exit' to quit.\n\n",
			   CLR_YELLOW_BOLD, CLR_RED_BOLD, CLR_YELLOW_BOLD);
	/* --------------------------------------------------------------------- */
	initialize_environ(); /* makes environ dynamically allocated */

	shellLoop(isInteractive, argv); /* main shell loop */

	/* ------------------- On exit - one time execution ------------------- */
	if (isInteractive)
		printf("%s\nThe %sGates Of Shell%s have closed. Goodbye.\n%s",
			   CLR_YELLOW_BOLD, CLR_RED_BOLD, CLR_YELLOW_BOLD, CLR_DEFAULT);

	return (EXIT_SUCCESS);
	/* -------------------------------------------------------------------- */
	/*
	 * note: 'On exit' code will probably never run unless something goes wrong.
	 * TODO: We should consider returning EXIT_FAILURE
	 *  or using break instead of exit(EXIT_SUCCESS)
	 */
}

/**
 * executeIfValid - check if a command is a valid custom or built-in command;
 * run the command if it is valid; if child process fails,stop it
 * from re-entering loop
 *
 * @isAtty: result of isatty(), 1 if interactive, 0 otherwsie
 * @argv: carrier of filename in [0]
 * @input: user-input
 * @tokens: array of strings of user inputs delimited by spaces
 */
void executeIfValid(int isAtty, char *const *argv, char *input, char **tokens)
{
	int custom_cmd_rtn;

	/* Handle built-in commands */
	custom_cmd_rtn = customCmd(tokens, isAtty);
	if (custom_cmd_rtn == 1)
	{
		return; /* Built-in command was handled, return to the main loop */
	}
	else if (custom_cmd_rtn == -1)
	{
		if (!isAtty) /*error occurred in non-interactive*/
		{
			safeExit(EXIT_FAILURE);
		}
		return;
	}

	/* Not a built-in command, try executing as external command*/
	if (tokens[0] == NULL)
	{
		return; /* Empty command - just return to the prompt */
	}

	char *full_path = findPath(tokens[0]);
	if (full_path == NULL)
	{
		fprintf(stderr, "%s: 1: %s: not found\n", argv[0], tokens[0]);
		if (!isAtty)
		{
			safeExit(127); /* Standard not found error status */
		}
		return;
	}

	int run_cmd_rtn = execute_command(full_path);
	free(full_path);

	if (run_cmd_rtn != 0)
	{
		/* Handle errors from execute_command */
		if (run_cmd_rtn == 127)
		{
			/* Already handled the "not found" case, but this is here for clarity and in case execute_command changes */
			fprintf(stderr, "%s: 1: %s: not found\n", argv[0], tokens[0]);
		}
		else if (run_cmd_rtn == -1)
		{ /* fork failed */
			perror("fork failed");
		}
		else
		{
			/* Other execve errors: use perror to print a descriptive message */
			fprintf(stderr, "%s: 1: %s: ", argv[0], tokens[0]);
			errno = run_cmd_rtn; /* Set errno, to error code */
			perror("");
		}

		if (!isAtty)
		{
			/* use run_cmd_rtn exit status. */
			safeExit(run_cmd_rtn);
		}
	}
}

/**
 * resetAll - frees all dynamically allotted memory to reset for next cmd
 * @tokens: array of strings needing free()
 * ...: list of variables to free
 */
void resetAll(char **tokens, ...)
{
	va_list vars;
	int i;
	char *free_me;

	if (tokens != NULL)
	{
		for (i = 0; tokens[i] != NULL; i++)
			if (tokens[i])
				free(tokens[i]);
		free(tokens);
	}
	va_start(vars, tokens);
	free_me = va_arg(vars, char *);
	while (free_me != NULL)
	{
		free(free_me);
		free_me = va_arg(vars, char *);
	}
	va_end(vars);
}

/**
 * safeExit - exits after freeing environ
 * @exit_code: exit code for exit()
 *
 * Return: void
 */
void safeExit(int exit_code)
{
	int i;

	if (environ)
	{
		for (i = 0; environ[i] != NULL; i++)
			free(environ[i]);
		free(environ);
		environ = NULL;
	}
	exit(exit_code);
}
