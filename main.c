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
	/* initialize_environ();  makes environ dynamically allocated */

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
 * @paths: array of strings of filepaths
 */
void executeIfValid(int isAtty, char *const *argv, char *input, char **tokens,
					char *cmd, char **paths)
{
	int run_cmd_rtn;

	/* Check for pipe */
	char *command1, *command2;
	if (split_command_line_on_pipe(input, &command1, &command2) == 0)
	{
		char **args1 = parse_command(command1);
		char **args2 = parse_command(command2);

		if (args1 && args2)
		{
			int pipeStatus = execute_pipe_command(args1, args2);
			if (pipeStatus != 0)
			{
				fprintf(stderr, "Pipe execution failed with status %d\n", pipeStatus);
			}
			free(args1);
			free(args2);
		}
		else
		{
			fprintf(stderr, "Failed to parse commands\n");
		}
		return;
	}

	/* Check for logical operators */
	if (strstr(input, "&&") || strstr(input, "||") || strstr(input, ";"))
	{
		execute_logical_commands(input);
		return;
	}

	/* Existing custom command handling */
	if (strcmp(tokens[0], "exit") == 0)
	{
		/* Handle exit command */
		safeExit(0);
	}
	else
	{
		/* Execute other commands */
		run_cmd_rtn = execute_command(tokens);
		if (run_cmd_rtn != 0)
		{
			fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
			if (!isAtty)
			{
				safeExit(run_cmd_rtn);
			}
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
	exit(exit_code);
}
