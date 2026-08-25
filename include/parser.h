#ifndef PARSER_H
#define PARSER_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

typedef struct s_flags {

    char **operands;
    size_t operands_count;
    bool l;
    bool r_lower;
    bool r_upper;
    bool t;
    bool a;
    bool has_error;
    bool help_used;

} t_flags;


void init_flags_struct(t_flags *opts, int ac);
void parse_args(char **av, t_flags *opts);
void clean_struct_flags(t_flags *opts);
void set_option(t_flags *opts, char option);

#endif