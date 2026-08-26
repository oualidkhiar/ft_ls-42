#include "../include/ft_ls.h"

typedef struct s_metadata {

    struct s_metadata   *next_depth;
    char                *name;
    struct stat         st;
    size_t              content_count;
    bool                is_dir;

} t_metadata;

bool get_file_details(char *fileName, struct stat *st)
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

size_t get_content_count(DIR *dir, bool count_hiden_files)
{
    size_t count = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.' && !count_hiden_files) {
            continue ;
        }
        count++;
    }

    rewinddir(dir);
    return count;
}

DIR *open_dir(char *dir_name)
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

t_metadata *get_dir_content(char *dirname, size_t *content_count, t_flags *opts)
{
    DIR *dir = open_dir(dirname);

    if (!dir) {
        opts->has_error = true;
        return NULL;
    }

    *content_count = get_content_count(dir, opts->a);

    t_metadata *Base = malloc(sizeof(t_metadata) * (*content_count));

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

    return Base;
}

void print_names(t_metadata *base, size_t size, int level, bool *printed)
{
    size_t i = 0;
    while (i < size)
    {
        if (base[i].name && !(base[i].is_dir && level == 0))
        {
            if (*printed)
                printf(" ");
            printf("%s", base[i].name);
            *printed = true;
        }
        i++;
    }
    if (*printed)
        printf("\n");
}

void print_list(t_metadata *base, size_t size, bool is_recursive, int level, bool first_section)
{
    bool printed = false;
    bool need_gap;
    size_t i;

    print_names(base, size, level, &printed);
    need_gap = printed || !first_section;
    i = 0;
    while (i < size)
    {
        if (base[i].next_depth)
        {
            if (is_recursive || size > 1)
            {
                if (need_gap)
                    printf("\n");
                printf("%s:\n", base[i].name);
                need_gap = true;
            }
            print_list(base[i].next_depth, base[i].content_count,
                       is_recursive, level + 1, false);
        }
        i++;
    }
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

void ls_entry(t_flags *opts)
{
    t_metadata *Base_node = operands_resolver(opts);
    // if (opts->has_error) {
    //     return ;
    // }
    // printf("herererre\n");
    print_list(Base_node, opts->operands_count, opts->r_upper, 0, true);
    

    clean_tree(Base_node, opts->operands_count);
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

















// void print_content_current_dir(t_metadata *node, size_t content_count)
// {
//     for (size_t i = 0; i < content_count; i++) {
//         if (node[i].is_dir)
//             printf(ANSI_BOLD_BLUE"%s"ANSI_RESET, node[i].name);
//         else
//             printf("%s", node[i].name);
//         if (i + 1 != content_count)
//             printf(" ");
//         else
//             printf("\n");
//     }
// }

// void print_list(t_metadata *node, size_t content_count, int depth_level, t_flags *opts, char *parent)
// {
//     size_t count = 0;
    
//     print_content_current_dir(node[count].next_depth, node[count].content_count);

//     while (count < content_count)
//     {
//         if (node[count].is_dir)
//         {
//             char *current_base = NULL;

//             if (depth_level != 0)
//                 printf("\n");

//             if (depth_level != 0 && parent && parent[0] != '\0') {

//                 char *normalized_base = ((parent[strlen(parent) - 1] != '/')) ? 
//                                     strjoin(parent, "/") : strdup(parent);

//                 current_base = strjoin(normalized_base, node[count].name);
//                 printf("%s:\n", current_base);
//                 free(normalized_base);
//             }

//             else { // first depth level
//                 printf("%s:\n", node[count].name);
//                 current_base = strdup(node[count].name);
//             }

//             if (opts->r_upper)
//                 print_list(node[count].next_depth, node[count].content_count, 
//                                             depth_level+1, opts, current_base); // recursively call print the content of other directories
//             free(current_base);
//         }

//         count++;
//     }
// }