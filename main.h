#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

extern char** environ;
#define MAX_INPUT_SIZE 1024

int _putchar(char c);
int print_string(char *str);
void execute_command(char *args[]);
void tokenize_input(char *input, char *args[]);


void print_environment(void);

#endif
