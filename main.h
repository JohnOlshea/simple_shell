#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>

#define MAX_INPUT_SIZE 1024
void custom_tokenize(char *input, char *args[]);
void execute_command(char *args[]);
void execute_directly(char *command, char *args[]);
void execute_with_fork(char *command, char *args[]);
void execute_in_path(char *command, char *args[]);

#endif
