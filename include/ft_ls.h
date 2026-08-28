#ifndef FT_LS_H
#define FT_LS_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include "parser.h"

#define DEFAULT_SIZE    10


typedef struct s_metadata {

    struct s_metadata   *next_depth;
    char                *name;
    struct stat         st;
    size_t              content_count;
    bool                is_dir;

} t_metadata;


void ls_entry(t_flags *opts, char **args, int args_len);
void clean_tree(t_metadata *base, size_t size);

#endif