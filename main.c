#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * main - Entry point
 *
 * Return: Always 0.
 */
int main(void)
{
	int i;
	size_t input_size = 0;
	ssize_t chars_read;
	char *input = NULL;
	char *args[MAX_INPUT_SIZE];

	while (1)
	{
		if (isatty(0))
		{
			write(STDOUT_FILENO, "($) ", 4);
			fflush(stdout);
			/*print_string("($) ");*/
		}

		chars_read = getline(&input, &input_size, stdin);
		if (chars_read < 0)
		{
			if (isatty(0))
				print_string("\n");
			free(input);
			exit(EXIT_FAILURE);
		}

		if (input[chars_read - 1] == '\n')
			input[chars_read - 1] = '\0';

		/* Ignore comments (lines starting with '#') */
		if (input[0] == '#') {
			continue;
		}

		tokenize_input(input, args);

		if (args[0] != NULL)
		{
			if (strcmp(args[0], "exit") == 0)
			{
				/* Free allocated memory before exiting */
				free(input);

				/* Check for exit status argument */
				if (args[1] != NULL)
				{
					int exit_status = atoi(args[1]);

					exit(exit_status);
				}
				else
				{
					exit(0);
				}
			}
			/* Check for the env built-in command */
			if (strcmp(args[0], "env") == 0)
				print_environment();
			/* Check for the setenv built-in command */
			else if (strcmp(args[0], "setenv") == 0)
			{
				if (args[1] != NULL && args[2] != NULL)
					set_environment_variable(args[1], args[2]);
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
					unset_environment_variable(args[1]);
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
				execute_command(args);
		}
		else
		{
			if (!isatty(0))
				exit(0);
		}
		/* Free memory for tokens */
		for (i = 0; args[i] != NULL; i++) {
			free(args[i]);
		}
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
		{
			print_string_err("Error: HOME environment variable not set\n");
			return;
		}
	}

	if (chdir(directory) == -1)
	{
		perror("chdir");
		print_string_err("Failed to change dirctor to ");
		print_string_err(directory);
	}
}

/**
 * set_environment_variable - Set environment variable
 *
 * @variable: varaible name
 * @value: value to set
 *
 */
void set_environment_variable(char *variable, char *value)
{
	if (setenv(variable, value, 1) == -1)
	{
		perror("setenv");
		print_string_err("Failed to set environment variable ");
		print_string_err(variable);
		print_string_err("\n");
	}
}

/**
 * unset_environment_variable - remove environment
 *
 * @variable: varaible name
 */
void unset_environment_variable(char *variable)
{
	if (unsetenv(variable) == -1)
	{
		perror("unsetenv");
		print_string_err("Failed to unset environment variable ");
		print_string_err(variable);
		print_string_err("\n");
	}
}

/**
 * execute_with_fork - Execute a command with fork, execve, and wait
 *
 * @command: x
 * @args: x
 */
void execute_with_fork(char *command, char *args[])
{
	int status;
	pid_t pid = fork();

	if (pid < 0)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		if (execve(command, args, environ) == -1)
		{
			if (!isatty(0))
			{
				print_string_err("./hsh: 1: ");
				print_string_err(command);
				print_string_err(": not found\n");
				exit(127);
			}
			perror(command);
			print_string("./hsh: No such file or directorya\n");
			exit(127);
		}
	}
	else
	{
		if (waitpid(pid, &status, 0) == -1)
		{
			perror("waitpid");
			exit(EXIT_FAILURE);
		}

		if (WIFEXITED(status))
		{
			/*exit_status = WEXITSTATUS(status);*/

			/*write(STDOUT_FILENO, "Exit status: ", 13);*/

			/*char exit_status_str[4];*/
			/*sprintf(exit_status_str, "%d", exit_status);*/
			/*write(STDOUT_FILENO, exit_status_str, strlen(exit_status_str));*/
			/*write(STDOUT_FILENO, "\n", 1);*/

			/*if (!isatty(0))*/
				/*exit(EXIT_SUCCESS);*/
		}		
	}
}

/**
 * print_environment - print environment variable
 *
 * Return: Always 0.
 */
void print_environment(void)
{
	char **env_var = environ;

	while (*env_var != NULL)
	{
		printf("%s\n", *env_var);
		env_var++;
	}
}

/**
 * execute_in_path - Search for the command in PATH and execute if found
 *
 * @command: command to execute
 * @args: array or argument and flags
 */
int execute_in_path(char *command, char *args[])
{
	char *path_env = getenv("PATH");
	char full_path[MAX_INPUT_SIZE];
	char *path_copy;
	char *token;

	if (path_env == NULL)
	{
		print_string(command);
		print_string(": ");
		print_string(args[0]);
		print_string(": not found\n");
		/*exit(EXIT_FAILURE);*/
		return (127);
	}

	path_copy = strdup(path_env);
	if (path_copy == NULL)
	{
		perror(command);
		exit(EXIT_FAILURE);
	}

	token = _strtok(path_copy, ":");
	while (token != NULL)
	{
		strcpy(full_path, token);
		strcat(full_path, "/");
		strcat(full_path, command);

		if (access(full_path, X_OK) == 0)
		{
			execute_with_fork(full_path, args);
			free(path_copy);
			return (0);
		}

		token = _strtok(NULL, ":");
	}

	/*
	print_string(command);
	print_string(": ");
	print_string(args[0]);
	print_string(": not found\n");
	free(path_copy);
	*/

	/*exit(EXIT_FAILURE);*/
	return (98);
}

/**
 * execute_directly - Execute a command directly using its full path
 * @command: command to execute
 * @args: array or argument and flags
 *
 */
void execute_directly(char *command, char *args[])
{
	execute_with_fork(command, args);
}

/**
 * execute_command - Execute a command, either directly or by searching in PATH
 * @args: array or argument and flags
 *
 */
int execute_command(char *args[])
{
	if (args[0][0] == '/' || args[0][0] == '.')
	{
		execute_directly(args[0], args);
	}
	else
	{
		execute_in_path(args[0], args);
	}

	if (!isatty(0))
		exit(0);
	return (EXIT_SUCCESS);
	/*exit(EXIT_SUCCESS);*/
}

/**
 * _putchar - writes the character c to stdout
 * @c: The character to print
 *
 * Return: On success 1.
 * On error, -1 is returned, and errno is set appropriately.
 */
int _putchar(char c)
{
	return (write(1, &c, 1));
}

/**
 * print_string - writes string to stdout
 * @str: The string to print
 *
 * Return: On success 1.
 * On error, -1 is returned, and errno is set appropriately.
 */
int print_string(char *str)
{
	int i = 0, count = 0;

	while (str[i])
	{
		count += _putchar(str[i++]);
	}
	return (count);
}


/**
 * tokenize_input - x
 * @input: x
 * @args: x
 *
 * Return: Always 0.
 */
char *_strtok(char *s, const char *delim)
{
	static char *temps;
	char *token;

	if (s == NULL)
	s = temps;

	s += strspn(s, delim);

	if (*s == '\0')
	{
		temps = s;
		return (NULL);
	}

	token = s;
	s = strpbrk(token, delim);
	if (s == NULL)
		temps = strchr(token, '\0');
	else
	{
		*s = '\0';
		temps = s + 1;
	}
	return (token);
}

/**
 * tokenize_input - x
 * @input: x
 * @args: x
 *
 * Return: Always 0.
 */
void tokenize_input(char *input, char *args[])
{
	int arg_count = 0;
	char *token = _strtok(input, " \n");

	while (token != NULL && arg_count < MAX_INPUT_SIZE - 1)
	{
		args[arg_count] = token;
		arg_count++;
		token = _strtok(NULL, " \n");
	}
	args[arg_count] = NULL;
}

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
