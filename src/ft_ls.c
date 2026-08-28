#include "../include/ft_ls.h"
#include "../include/display.h"

static bool get_file_details(char *fileName, struct stat *st)
{
    if (lstat(fileName, st) == -1)
    {
        if (errno == EACCES) {
            fprintf(stderr, "ls: cannot open directory '%s': Permission denied\n", fileName);
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

static DIR *open_dir(char *dir_name)
{
    DIR *dir = opendir(dir_name);
    
    if (dir == NULL) {
        if (errno == EACCES) {
            fprintf(stderr, "ls: cannot open directory '%s': Permission denied\n", dir_name);
        }
        return NULL;
    }
    return dir;
}

static t_metadata *get_dir_content(char *dirname, size_t *content_count, t_flags *opts)
{
    DIR *dir = open_dir(dirname);

    if (!dir) {
        opts->has_error = true;
        return NULL;
    }

    struct dirent *entry; 

    size_t Base_index = 0;
    size_t capacity = 1;

    char *dir_name;
    
    t_metadata *Base = malloc(sizeof(t_metadata) * DEFAULT_SIZE);
    
    dir_name = (dirname[strlen(dirname) - 1] != '/') ? 
        strjoin(dirname, "/") : strdup(dirname);

    while ((entry = readdir(dir)) != NULL) {

        t_metadata current_node = {0};

        char *full_path = strjoin(dir_name, entry->d_name);

        if (entry->d_name[0] == '.' && !opts->a) {
            free(full_path);
            continue;
        }

        if (!get_file_details(full_path, &(current_node.st))) {
            free(full_path);
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

        if (Base_index == (DEFAULT_SIZE * capacity)) {
            t_metadata *tmp = realloc(
                                        Base, 
                                        sizeof(t_metadata) * 
                                        (DEFAULT_SIZE * 
                                        (++capacity))
                                    );
            Base = tmp;
        }

        Base[Base_index++] = current_node;
        free(full_path);
    }

    *content_count = Base_index;
    closedir(dir);
    free(dir_name);

    return Base;
}

static t_metadata *operands_resolver(t_flags *opts, size_t *content_count)
{
    t_metadata *Base = malloc(sizeof(t_metadata) * opts->operands_count);

    size_t Base_index = 0;

    for (size_t i = 0; i < opts->operands_count; i++) {

        t_metadata current_node = {0};

        if (!get_file_details(opts->operands[i], &(current_node.st))) {
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

    *content_count = Base_index;

    return Base;
}

void clean_tree(t_metadata *base, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        if (base[i].next_depth) {
            clean_tree(base[i].next_depth, base[i].content_count);
        }
        free(base[i].name);
    }
    free(base);
}

void ls_entry(t_flags *opts, char **args, int args_len)
{
    init_flags_struct(opts, args_len);
    parse_args(args, opts);
    if (
        opts->help_used ||
        opts->has_error
    )
        return ;

    size_t content_count = 0;

    t_metadata *Base_node = operands_resolver(opts, &content_count);

    // if (opts->)
    print_list(Base_node, opts, content_count, 0, true, NULL);
    clean_tree(Base_node, content_count);
}