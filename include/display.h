#ifndef DISPLAY_H
#define DISPLAY_H

#include <time.h>
#include "ft_ls.h"

#define BLUE            "\033[1;34m"  // blue color for directories
#define GREEN           "\033[01;32m"    // green color for executble files
#define RESET           "\033[0m"

void print_names(t_metadata *base, t_flags *opts, size_t size, int level, bool *printed);
void print_list(t_metadata *base, t_flags *opts, size_t size, int level, bool first_section, char *parent_level);

#endif