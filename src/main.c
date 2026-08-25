#include "../include/ft_ls.h"
#define ANSI_BOLD_BLUE  "\033[1;34m"
#define ANSI_RESET      "\033[0m"

typedef struct s_metadata {

    struct s_metadata   *next_depth;
    struct stat         st;
    char                *name;
    size_t              content_count;
    bool                is_dir;

} t_metadata;

bool get_file_details(char *fileName, t_metadata *current_node)
{
    if (lstat(fileName, &(current_node->st)) == -1)
    {
        if (errno == EACCES) {
            printf("Error: Permission denied. You do not have access to this path.\n");
        }
        else if (errno == ENOENT) {
            fprintf(stderr, "ft_ls: cannot access '%s': No such file or directory\n",
            fileName);
        }
        else {
            perror("lstat failed\n");
            exit(1);
        }
        return false;
    }
    return true;
}

size_t get_content_count(char *dirname, bool count_hiden_files)
{
    size_t count = 0;
    struct dirent *entry;

    DIR *dir = opendir(dirname);

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.' && !count_hiden_files) {
            continue ;
        }
        count++;
    }

    closedir(dir);
    return count;
}

t_metadata *get_dir_content(char *dirname, size_t *content_count, t_flags *opts)
{
    *content_count = get_content_count(dirname, opts->a);

    t_metadata *Base = malloc(sizeof(t_metadata) * (*content_count));

    DIR *dir = opendir(dirname);
    
    if (dir == NULL) {
        perror("Unable to open directory");
        opts->has_error = true;
        return NULL;
    }

    struct dirent *entry; 

    size_t Base_index = 0;

    char *dir_name;

    dir_name = (dirname[strlen(dirname) - 1] != '/') ? 
        strjoin(dirname, "/") : strdup(dirname);

    while ((entry = readdir(dir)) != NULL) {

        t_metadata current_node = {0};

        char *full_path = strjoin(dir_name, entry->d_name);

        if (entry->d_name[0] == '.' && !opts->a) {
            free(full_path);
            continue;
        }

        if (!get_file_details(full_path, &current_node)) {
            opts->has_error = true;
            continue ;
        }

        current_node.name = strdup(entry->d_name);

        if (S_ISDIR(current_node.st.st_mode) && not_current_or_parent_dir(current_node.name)) {
            if (opts->r_upper) {
                current_node.next_depth = 
                    get_dir_content(full_path, &(current_node.content_count), opts);
            }
            current_node.is_dir = true;
        }

        Base[Base_index++] = current_node;
        free(full_path);
    }

    closedir(dir);
    free(dir_name);

    return Base;
}

t_metadata *operands_resolver(t_flags *opts)
{
    t_metadata *Base = malloc(sizeof(t_metadata) * opts->operands_count);

    size_t Base_index = 0;

    for (size_t i = 0; i < opts->operands_count; i++) {

        t_metadata current_node = {0};

        if (!get_file_details(opts->operands[i], &current_node)) {
            opts->has_error = true;
            continue ;
        }


        current_node.name = strdup(opts->operands[i]);

        if (S_ISDIR(current_node.st.st_mode)) {
            current_node.next_depth = 
                get_dir_content(current_node.name, &current_node.content_count, opts);
            current_node.is_dir = true;
        }

        Base[Base_index++] = current_node;
    }

    return Base;
}

void print_list(t_metadata *node, size_t content_count, int level, t_flags *opts, char *parent)
{
    size_t count = 0;
    bool printed = false;

    if (level != 0) {
        for (size_t i = 0; i < content_count; i++) {
            if (node[i].is_dir)
                printf(ANSI_BOLD_BLUE"%s"ANSI_RESET, node[i].name);
            else
                printf("%s", node[i].name);
            if (i + 1 != content_count)
                printf(" ");
            else
                printf("\n");
        }
    }

    while (count < content_count)
    {
        if (node[count].next_depth && opts->r_upper) {

            char *next_parent = NULL;

            if (level != 0 && parent && parent[0] != '\0') {

                printf("\n");

                char *current_base = ((parent[strlen(parent) - 1] != '/')) ? 
                                        strjoin(parent, "/") : strdup(parent);

                printf("%s:\n", current_base);
                free(current_base);

            }

            else {
                printf("%s:\n", node[count].name);
            }

            print_list(node[count].next_depth, node[count].content_count, level+1, opts, parent);
            free(parent);

        }

        count++;
    }
}

void ls_entry(t_flags *opts)
{
    t_metadata *Base_node = operands_resolver(opts);
    // if (opts->has_error) {
    //     return ;
    // }
    // printf("herererre\n");
    print_list(Base_node, opts->operands_count, 0, opts, NULL);

}

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