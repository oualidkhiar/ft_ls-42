#include "../include/parser.h"

void init_flags_struct(t_flags *opts, int ac)
{
    opts->operands          = malloc(sizeof(char *) * ac);
    opts->operands_count    = 0;
    opts->l                 = false;
    opts->r_lower           = false;
    opts->r_upper           = false;
    opts->t                 = false;
    opts->a                 = false;
    opts->has_error         = false;
    opts->help_used         = false;
}

void set_option(t_flags *opts, char option)
{
    if (option == 'R') {
        opts->r_upper = true;
    } 
    else if (option == 'r') {
        opts->r_lower = true;
    }
    else if (option == 'l') {
        opts->l = true;
    }
    else if (option == 't') {
        opts->t = true;
    }
    else if (option == 'a') {
        opts->a = true;
    }
    else {
        fprintf(stderr, "ls: invalid option -- '%c'\nTry 'ls --help' for more information.\n", option);
        opts->has_error = true;
    }
}

void parse_args(char **av, t_flags *opts)
{
    int i = 0;

    while (av[i]) {

        int j = 0;

        while (av[i][j]) {

            if (is_white_space(av[i][j])) {
                j++;
                continue ;
            }
            
            if (av[i][j] == '-') {

                j += 1;

                if (strcmp(av[i], "--help") == 0) {
                    ft_ls_help();
                    clean_struct_flags(opts);
                    opts->help_used = true;
                    return ;
                }

                while (av[i][j])
                {
                    set_option(opts, av[i][j]);
                    if (opts->has_error) {
                        return ;
                    }
                    j++;
                }
            }
            else {
                size_t len = strlen(av[i]) + 1;
                opts->operands[opts->operands_count] = malloc(sizeof(char) * len);
                memcpy(opts->operands[opts->operands_count], av[i], len);
                opts->operands_count++;
                break ;
            }
        }
        i++;
    }
    
    if (opts->operands_count == 0) {
        opts->operands[0] = malloc(sizeof(char) * 2);
        opts->operands[0][0] = '.';
        opts->operands[0][1] = '\0';
        opts->operands_count++;;
    }
}

void clean_struct_flags(t_flags *opts)
{
    for (size_t i = 0; i < opts->operands_count; i++) {
        free(opts->operands[i]);
    }
    free(opts->operands);
}