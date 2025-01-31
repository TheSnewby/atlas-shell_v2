#ifndef MAIN_H
#define MAIN_H

/* ↓ LIBRARIES ↓ */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <linux/limits.h>
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
/* ------------------- */

/* ↓ FUNCTIONS ↓ */
int isNumber(char *number);

int _atoi(const char *s);

void shellLoop(int isAtty, char *argv[]);

void selfDestruct(int countdown);

void saveInput(int isAtty, char **tokens, size_t *size, char **input);

int parseInput(char *input, char ***tokens, char **cmd_token,
					int *tokens_count);

void executeIfValid(int isAtty, char *const *argv, char *input, char **tokens,
					char *cmd, char *cmd_token, char **paths);

void initCmd(char **cmd, char *const *tokens);

int populateTokens(const char *input, char ***tokens, char **cmd_token,
				   int *tokens_count);

void freeAll(char **tokens, ...);

void ifCmdExit(char **tokens, int interactive, const char *f1, const char *f2,
			   const char *f3);

int ifCmdSelfDestruct(char **tokens, const char *f1, const char *f2,
					  const char *f3);
int customCmd(char **tokens, int interactive, char *f1, char *f2, char *f3);

int runCommand(char *commandPath, char **args, char **envPaths);

char *_getenv(const char *name);

void printPrompt(int isAtty, char *user, char *hostname, char *path);

path_t *buildListPath(void);

char *getUser();

char *getHostname();

int _setenv(const char *name, const char *value, int overwrite);

int _unsetenv(const char *name);

char *findPath(char *name);

void destroyListPath(path_t *h);

int ifCmdEnv(char **tokens);

int ifCmdSetEnv(char **tokens);

int ifCmdUnsetEnv(char **tokens);
/* ------------------- */

#endif /* MAIN_H */
