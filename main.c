#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

#define MAX_INPUT_SIZE 1024

/**
 * _putchar_err - writes the character c to stderr
 * @c: The character to print
 *
 * Return: On success 1.
 * On error, -1 is returned, and errno is set appropriately.
 */
int _putchar_err(char c)
{
	return (write(2, &c, 1));
}

/**
 * print_string_err - writes string to stderr
 * @str: The string to print
 *
 * Return: On success 1.
 * On error, -1 is returned, and errno is set appropriately.
 */
int print_string_err(char *str)
{
	int i = 0, count = 0;

	while (str[i])
	{
		count += _putchar_err(str[i++]);
	}
	return (count);
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
 * execute_with_fork - x
 *
 * @variable: varaible name
 * @value: value to set
 *
 * Return: Always 0.
 */
int execute_with_fork(char *command, char *args[])
{
	pid_t pid = fork();
	int status;

	if (pid < 0)
	{
		perror(command);
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		if (execve(command, args, environ) == -1)
		{
			fprintf(stderr, "./hsh: 1: %s: not found\n", command);
			return (127);
		}
	}
	else
	{
		if (wait(&status) == -1)
		{
			perror(command);
			exit(EXIT_FAILURE);
		}
		return (0);
	}
	return (127);
}


/**
 * set_environment_variable - Set environment variable
 *
 * @variable: varaible name
 * @value: value to set
 *
 * Return: Always 0.
 */
int execute_in_path(char *command, char *args[])
{
	char *path_env = getenv("PATH");
	char full_path[MAX_INPUT_SIZE];
	char *token;
	char path_copy[MAX_INPUT_SIZE];
	int exit_status;

	if (path_env == NULL)
	{
		fprintf(stderr, "./hsh: 1: %s: not found\n", command);
		exit(127);
	}

	strcpy(path_copy, path_env);

	if (path_copy == NULL)
	{
		perror(command);
		exit(EXIT_FAILURE);
	}

	token = strtok(path_copy, ":");

	while (token != NULL)
	{
	snprintf(full_path, sizeof(full_path), "%s/%s", token, command);

	if (access(full_path, X_OK) == 0)
	{
		exit_status = execute_with_fork(full_path, args);
		return (exit_status);
	}

	token = strtok(NULL, ":");
	}

	fprintf(stderr, "./hsh: 1: %s: not found\n", command);
	return (127);
}

/**
 * execute_command - x
 *
 * args - x
 */
int execute_command(char *args[])
{
	if (args[0][0] == '/' || args[0][0] == '.')
		return (execute_with_fork(args[0], args));
	else
		return (execute_in_path(args[0], args));
}

/**
 * custom_tokenize - custom strtok
 *
 * @input: string to tokenize
 * @args: tokenized array
 */
void custom_tokenize(char *input, char *args[])
{
	int arg_count = 0;
	char *token = strtok(input, " \n");

	while (token != NULL && arg_count < MAX_INPUT_SIZE - 1)
	{
		args[arg_count] = token;
		if (args[arg_count] == NULL)
		{
			perror("strdup");
			exit(EXIT_FAILURE);
		}
		arg_count++;
		token = strtok(NULL, " \n");
	}

	/* Null-terminate the args array */
	args[arg_count] = NULL;
}

/**
 * print_environment - print environment variable
 *
 * Return: Always 0.
 */
int print_environment(void)
{
	extern char **environ;
	char **env = environ;

	while (*env != NULL)
	{
		printf("%s\n", *env);
		env++;
	}
	return (0);
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
	char *input = NULL;
	char *newline;
	char *args[MAX_INPUT_SIZE];
	size_t input_size = 0;
	int exit_status;
	int exec_exit_status;
	char *comment_start;
	int i;

	while (1)
	{
		if (isatty(0))
		{
			write(STDOUT_FILENO, "($)  ", 4);
			fflush(stdout);
		}

		if (getline(&input, &input_size, stdin) == -1)
		{
			if (isatty(0))
			write(STDOUT_FILENO, "\n", 1);
			exit(EXIT_FAILURE);
		}

		/* Remove trailing newline */
		newline = strchr(input, '\n');

		if (newline != NULL)
			*newline = '\0';

		comment_start = strchr(input, '#');
		if (comment_start != NULL)
			*comment_start = '\0';

		/* Tokenize the input */
		custom_tokenize(input, args);
		if (args[0] != NULL)
		{
			/* Check for the exit built-in command */
			if (strcmp(args[0], "exit") == 0)
			{
				/* Free allocated memory before exiting */
				free(input);

				/* Check for exit status argument */
				if (args[1] != NULL)
				{
					exit_status = atoi(args[1]);

					exit(exit_status);
				}
				else
					exit(EXIT_SUCCESS);
			}

			/* Check for the env built-in command */
			if (strcmp(args[0], "env") == 0)
			{
				extern char **environ;
				char **env = environ;

				while (*env != NULL)
				{
					printf("%s\n", *env);
					env++;
				}

				if (!isatty(0))
					return (0);
			}
			/* Check for the setenv built-in command */
			else if (strcmp(args[0], "setenv") == 0)
			{
				if (args[1] != NULL && args[2] != NULL)
				{
					set_environment_variable(args[1], args[2]);
				}
				else
				{
					char error_message[] = "Usage: setenv VARIABLE VALUE\n";

					write(STDERR_FILENO, error_message, sizeof(error_message) - 1);
				}
			}
			/* Check for the unsetenv built-in command */
			else if (strcmp(args[0], "unsetenv") == 0)
			{
				if (args[1] != NULL)
				{
				unset_environment_variable(args[1]);
				}
				else
				{
					char error_message[] = "Usage: unsetenv VARIABLE\n";

					write(STDERR_FILENO, error_message, sizeof(error_message) - 1);
				}
			}
			else if (strcmp(args[0], "cd") == 0)
			{
				change_directory(args[1]);
			}
			else
			{
				exec_exit_status = execute_command(args);
				if (!isatty(0))
				{
					return (exec_exit_status);
				}
			}
		}
		else
		{
			if (!isatty(0))
				return (0);
		}
		/* Free memory for tokens */
		for (i = 0; args[i] != NULL; i++) {
			free(args[i]);
		}		
	}

	return (0);
}
