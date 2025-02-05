#ifndef MAIN_H
#define MAIN_H

/* ↓ LIBRARIES ↓ */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <sys/types.h>
#include "colors.h"
/* ------------------- */

/* ↓ STRUCTS AND MISC ↓ */
/**
 * struct path_s - singly linked list for PATH environ variable
 * @directory: string of directory
 * @next: next location directory
 *
 * Description: singly linked list node structure
 */
typedef struct path_s
{
	char *directory;
	struct path_s *next;
} path_t;
extern char **environ;
// char **allocated_vars = NULL;
// int allocated_count = 0;
/* ------------------- */

/* ↓ FUNCTIONS ↓ */
int isNumber(char *number);

int _atoi_safe(const char *s);

void shellLoop(int isAtty, char *argv[]);

void selfDestruct(int countdown);

void saveInput(int isAtty, size_t *size, char **input);

int parseInput(char *input, char ***tokens, char **cmd_token,
			   int *tokens_count);

void executeIfValid(int isAtty, char *const *argv, char *input, char **tokens);

void initCmd(char **cmd, char *const *tokens);

int populateTokens(const char *input, char ***tokens, char **cmd_token,
				   int *tokens_count);

void resetAll(char **tokens, ...);

void ifCmdExit(char **tokens, int interactive, const char *f1, const char *f2,
			   const char *f3);

int ifCmdSelfDestruct(char **tokens, const char *f1, const char *f2,
					  const char *f3);
int customCmd(char **tokens, int interactive, char *f1, char *f2, char *f3);

int runCommand(char *commandPath, char **args);

char *_getenv(const char *name);

void printPrompt(int isAtty, char *user, char *hostname, char *path);

path_t *buildListPath(void);

char *getUser(void);

char *getHostname(void);

int _setenv(const char *name, const char *value, int overwrite);

int _unsetenv(const char *name);

char *findPath(char *name);

void destroyListPath(path_t *h);

int ifCmdEnv(char **tokens);

int ifCmdSetEnv(char **tokens);

int ifCmdUnsetEnv(char **tokens);

int ifCmdCd(char **tokens);

void initialize_environ(void);

void safeExit(int exit_code);

char *_strcat(char *dest, const char *src);

char *_strchr(const char *s, char c);

int _strlen(const char *s);

int _strcmp(const char *s1, const char *s2);

int _strncmp(const char *s1, const char *s2, int n);

char *_strcpy(char *dest, const char *src);

void *_realloc_array(char **ptr, unsigned int new_size);

char *_strdup(char *str);

/* pipe functions */
int split_command_line_on_pipe(char *line, char **command1, char **command2);
char **parse_command(char *command);
int execute_pipe_command(char **command1, char **command2);

/* logical operators functions */
int execute_command(char **args);

void execute_logical_commands(char *line);
typedef enum
{
	SEP_NONE,
	SEP_SEMICOLON,
	SEP_AND,
	SEP_OR
} SeparatorType;

extern char **environ;
extern char **saved_environ;

/* ------------------- */

#endif /* MAIN_H */
