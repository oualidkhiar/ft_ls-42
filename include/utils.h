#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ANSI_BOLD_BLUE  "\033[1;34m"
#define ANSI_RESET      "\033[0m"


char *strjoin(const char *s1, const char *s2);
bool not_current_or_parent_dir(const char *path);
bool is_white_space(char c);
void ft_ls_help();

#endif