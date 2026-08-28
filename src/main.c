#include "../include/ft_ls.h"

int main(int ac, char **av)
{
    t_flags opts;

    ls_entry(&opts, av + 1, ac);

    int exit_code = opts.has_error ? 2 : 0;

    clean_struct_flags(&opts);

    return exit_code;
}