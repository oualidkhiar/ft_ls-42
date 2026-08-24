#include "../include/ft_ls.h"
#include <errno.h>

typedef struct s_metadata {

    struct s_metadata   *next_depth;
    struct stat         st;
    char                *name;
    size_t              content_count;

} t_metadata;

t_metadata *get_content_of_next_depth(t_flags *opts, size_t index)
{

}

t_metadata *operands_resolver(t_flags *opts)
{
    t_metadata *Base = malloc(sizeof(t_metadata) * opts->operands_count);
    size_t Base_index = 0;

    for (size_t i = 0; i < opts->operands_count; i++) {

        t_metadata current_node = {0};

        if (lstat(opts->operands[i], &current_node.st) == -1)
        {
            if (errno == EACCES) {
                printf("Error: Permission denied. You do not have access to this path.\n");
            }
            else if (errno == ENOENT) {
                fprintf(stderr, "ft_ls: cannot access '%s': No such file or directory\n",
                opts->operands[i]);
            }
            else {
                perror("lstat failed");
                exit(1);
            }
            opts->has_error = true;
            continue;
        }

        current_node.name = opts->operands[i];

        if (S_ISDIR(current_node.st.st_mode)) {

            current_node.name = opts->operands[i];

            if (opts->r_upper) { // -R and -a are the only option we gonna check here because thy change the data ex {-a adds the hiden files } 
                current_node.next_depth = get_content_of_next_depth(opts, &current_node.content_count, );
            }
            else {
                // TODO: open dir readdir hold content of just one level
            }
        }

        Base[Base_index++] = current_node;
    }

    return Base;
}

void ls_entry(t_flags *opts)
{
    t_metadata *mtdt = operands_resolver(opts);
    if (opts->has_error) {
        return ;
    }

    for (int i = 0; i < opts->operands_count; i++) {
        printf("%s", mtdt[i].name);
        if (i + 1 != opts->operands_count)
            printf("  ");
    }

}


int main(int ac, char **av)
{
    t_flags opts;

    init_flags_struct(&opts, ac);

    parse_args(av + 1, &opts);

    if (opts.has_error) {
        clean_struct_flags(&opts);
        return 2;
    }

    ls_entry(&opts);

    int exit_code = opts.has_error ? 2 : 0;

    clean_struct_flags(&opts);

    return exit_code;
}