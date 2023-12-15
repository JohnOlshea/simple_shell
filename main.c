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
 * print_environment - Print environment variables in reverse order.
 */
void print_environment(void)
{
	char **env = environ;
	char **env_array;
	int count = 0;
	int i;

	/* Count the number of environment variables */
	while (*env != NULL)
	{
		count++;
		env++;
	}

	/* Allocate an array to store environment variables */
	env_array = malloc((count + 1) * sizeof(char *));
	if (env_array == NULL)
	{
		perror("Memory allocation error");
		exit(EXIT_FAILURE);
	}

	/* Copy environment variables to the array */
	env = environ;
	for (i = 0; i < count; i++)
	{
		env_array[i] = *env;
		env++;
	}

	env_array[count] = NULL;

	for (i = count - 1; i >= 0; i--)
		printf("%s\n", env_array[i]);

	free(env_array);
}

/**
 * set_environment_variable - Set environment variable
 *
 * @variable: varaible name
 * @value: value to set
 *
 * Return: Always 0.
 */
void set_environment_variable(char *variable, char *value)
{
	if (setenv(variable, value, 1) == -1)
	{
		perror("setenv");
		fprintf(stderr, "Failed to set environment variable %s\n", variable);
	}
}

/**
 * unset_environment_variable - Set environment variable
 *
 * @variable: varaible name
 *
 * Return: Always 0.
 */
void unset_environment_variable(char *variable)
{
	if (unsetenv(variable) == -1)
	{
		perror("unsetenv");
		fprintf(stderr, "Failed to unset environment variable %s\n", variable);
	}
}


/**
 * change_directory - change directory
 *
 * @directory: directory to change into
 *
 * Return: Always 0.
 */
void change_directory(char *directory)
{
if (directory == NULL || strcmp(directory, "-") == 0)
{
	directory = getenv("HOME");

	if (directory == NULL)
		return;
}

	if (chdir(directory) == -1)
		fprintf(stderr, "./hsh: 1: cd: can't cd to %s\n", directory);
}

/**
 * main - Simple shell
 *
 * Return: Always 0.
 */
int main(void)
{
	char *command = NULL;
	char *comment_start;
	int num_read;
	size_t n = 0;
	char **argv = NULL;
	pid_t child_pid;
	int status;
	int found_path;
	int exit_status;
	char **env = environ;

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
comment_start = strchr(command, '#');
if (comment_start != NULL)
*comment_start = '\0';		

		argv = tokenize(command, " \n\t");

		if (argv[0] != NULL)
		{
			/*built in*/
			if (argv[0][0] != '/' && argv[0][0] != '.')
			{

				if (strcmp(argv[0], "exit") == 0)
				{
					exit_status = 0;

					if (argv[1] != NULL)
						exit_status = atoi(argv[1]);

					free(command);
					free_tokens_array(argv);
					return (exit_status);
				}



else if (strcmp(argv[0], "cd") == 0)
{
	change_directory(argv[1]);
	continue;
}


			/* Check for the setenv built-in command */
			else if (strcmp(argv[0], "setenv") == 0)
			{
				if (argv[1] != NULL && argv[2] != NULL)
				{
					set_environment_variable(argv[1], argv[2]);
					continue;
				}
				else
				{
					char error_message[] = "Usage: setenv VARIABLE VALUE\n";

					write(STDERR_FILENO, error_message, sizeof(error_message) - 1);
				}
			}
			/* Check for the unsetenv built-in command */
			else if (strcmp(argv[0], "unsetenv") == 0)
			{
				if (argv[1] != NULL)
				{
					unset_environment_variable(argv[1]);
					continue;
				}
				else
				{
					char error_message[] = "Usage: unsetenv VARIABLE\n";

					write(STDERR_FILENO, error_message, sizeof(error_message) - 1);
				}
			}
			else if (strcmp(argv[0], "cd") == 0)
			{
				change_directory(argv[1]);
			}


                        /* Check for the env built-in command */
                        if (strcmp(argv[0], "env") == 0)
                        {

                                while (*env != NULL)
                                {
                                        printf("%s\n", *env);
                                        env++;
                                }

                                if (!isatty(0))
                                        return (0);
                        }




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
