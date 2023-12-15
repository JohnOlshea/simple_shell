#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_PATH_LENGTH 4096

extern char **environ;

/**
 * free_token_array - a
 * @tokens:  b
 *
 */
void free_tokens_array(char **tokens)
{
	int i = 0;

	while (tokens[i] != NULL)
	{
		free(tokens[i]);
		i++;
	}
	free(tokens);
}

/**
 * construct_path - x
 *
 * @command: y
 *
 * Return: Always 0.
 */

int construct_path(char **command)
{
	char *path = getenv("PATH");
	char full_path[MAX_PATH_LENGTH];
	char *token;
	char *original_command;

	if (path == NULL || path[0] == '\0')
		return (127);

	original_command = strdup(command[0]);
	if (original_command == NULL)
	{
		perror("Memory allocation error");
		exit(EXIT_FAILURE);
	}

	token = strtok(path, ":");

	while (token != NULL)
	{
		snprintf(full_path, sizeof(full_path), "%s/%s", token, original_command);

		if (access(full_path, F_OK | X_OK) == 0)
		{
			free(original_command);

			free(command[0]);
			command[0] = strdup(full_path);
			if (command[0] == NULL)
			{
				perror("Memory allocation error");
				exit(EXIT_FAILURE);
			}
			return (EXIT_SUCCESS);
		}

		token = strtok(NULL, ":");
	}

	free(original_command);

	return (EXIT_FAILURE);
}

/**
 * main - Simple shell
 *
 * Return: Always 0.
 */
int main(void)
{
	char *command = NULL;
	int num_read;
	size_t n = 0;
	char **argv = NULL;
	pid_t child_pid;
	int status;
	int found_path;

	while (1)
	{
		if (isatty(0))
		{
			write(STDOUT_FILENO, "($) ", 4);
			fflush(stdout);
		}

		num_read = getline(&command, &n, stdin);
		if (num_read == -1)
		{
			if (isatty(0))
				write(STDOUT_FILENO, "\n", 1);
			free(command);
			return (EXIT_SUCCESS);
		}
		command[strcspn(command, "\n")] = '\0';
		/*exit*/

		argv = tokenize(command, " \n\t");

		if (argv[0] != NULL)
		{
			/*built in*/
			if (argv[0][0] != '/' && argv[0][0] != '.')
			{
				found_path = construct_path(argv);
				if (found_path != 0)
				{
					fprintf(stderr, "./hsh: 1: %s: not found\n", argv[0]);
					if (!isatty(0))
					{
						free(command);
						free_tokens_array(argv);
						return (127);
					}
				}
			}
			else
			{
				if (access(argv[0], F_OK | X_OK) == -1)
				{

					fprintf(stderr, "./hsh: 1: %s: not found\n", argv[0]);
					free_tokens_array(argv);
					if (!isatty(0))
						return (127);

					continue;
				}
			}

			child_pid = fork();
			if (child_pid == -1)
			{
				free_tokens_array(argv);
				exit(EXIT_FAILURE);
			}
			if (child_pid == 0)
			{
				if (execve(argv[0], argv, environ) == -1)
					exit(EXIT_FAILURE);
			}
			else
			{
				free_tokens_array(argv);
				wait(&status);
			}
		}
		else
		{
			free_tokens_array(argv);
			if (!isatty(0))
				return (0);
		}
	}

	free(command);
	return (0);
}
