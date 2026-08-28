#include "../include/ft_ls.h"

int main(int ac, char **av)
{
    t_flags opts;

    init_flags_struct(&opts, ac);

    parse_args(av + 1, &opts);

    if (opts.help_used) {
        return 0;
    }

    if (opts.has_error) {
        clean_struct_flags(&opts);
        return 2;
    }

    ls_entry(&opts);

    int exit_code = opts.has_error ? 2 : 0;

    clean_struct_flags(&opts);

    return exit_code;
}