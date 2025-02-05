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
	(void) argc;
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
 * @cmd: first argument of user-input prefixed with found filepath
 * @cmd_token: last arguments from strtok'd input
 * @paths: array of strings of filepaths
 */
void executeIfValid(int isAtty, char *const *argv, char *input, char **tokens,
					char *cmd, char *cmd_token, char **paths)
{
	int run_cmd_rtn;

	/* run command */
	if (customCmd(tokens, isAtty, input, cmd, cmd_token, argv) == 0)
	{ /* if input is not a custom command */
		/* runs the command if it is a valid built-in */
		run_cmd_rtn = runCommand(cmd, tokens, paths);
		/* prints error if command is invalid or another error occurs */
		if (run_cmd_rtn != 0)
		{
			if (run_cmd_rtn == 127)
				fprintf(stderr, "%s: 1: %s: %s\n", argv[0], cmd,
					"not found");
			else if (run_cmd_rtn == 2)
				;
			else
				fprintf(stderr, "%s: 1: %s: %s\n", argv[0], cmd,
						strerror(run_cmd_rtn));
			if (!isAtty)
			{
				resetAll(tokens, input, cmd, NULL);
				safeExit(run_cmd_rtn);
			}
		}
	}

	resetAll(tokens, cmd, input, NULL);  /* resets for next user-input */
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
	}

	exit(exit_code);
}
