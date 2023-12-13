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
	size_t input_size = 0;
	ssize_t chars_read;
	char *input = NULL;
	char *args[MAX_INPUT_SIZE];

	while (1)
	{
		if (isatty(0))
		print_string("($) ");

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

		tokenize_input(input, args);

		if (args[0] != NULL)
		{
			execute_command(args);
		}
	}

	free(input);
	return (0);
}

/**
 * execute_with_fork - Execute a command with fork, execve, and wait
 * @command: x
 * @args: x
 *
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
				print_string("./hsh: 1: ");
				print_string(command);
				print_string(": not found\n");
				exit(127);
			}
			perror(command);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		if (wait(&status) == -1)
			exit(EXIT_FAILURE);
	}
}

/**
 * execute_in_path - Search for the command in PATH and execute if found
 *
 * @command: command to execute
 * @args: array or argument and flags
 */
void execute_in_path(char *command, char *args[])
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
		exit(EXIT_FAILURE);
	}

	path_copy = strdup(path_env);
	if (path_copy == NULL)
	{
		perror(command);
		exit(EXIT_FAILURE);
	}

	token = strtok(path_copy, ":");
	while (token != NULL)
	{
		strcpy(full_path, token);
		strcat(full_path, "/");
		strcat(full_path, command);

		if (access(full_path, X_OK) == 0)
		{
			execute_with_fork(full_path, args);
			free(path_copy);
			return;
		}

		token = strtok(NULL, ":");
	}

	print_string(command);
	print_string(": ");
	print_string(args[0]);
	print_string(": not found\n");
	free(path_copy);
	exit(EXIT_FAILURE);
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
void execute_command(char *args[])
{
	if (args[0][0] == '/' || args[0][0] == '.')
	{
		execute_directly(args[0], args);
	}
	else
	{
		execute_in_path(args[0], args);
	}
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
void tokenize_input(char *input, char *args[])
{
	int arg_count = 0;
	char *token = strtok(input, " \n");

	while (token != NULL && arg_count < MAX_INPUT_SIZE - 1)
	{
		args[arg_count] = token;
		arg_count++;
		token = strtok(NULL, " \n");
	}
	args[arg_count] = NULL;
}
