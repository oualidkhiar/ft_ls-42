#include "ft_ls.h"

static void print_names(t_metadata *base, size_t size, int level, bool *printed)
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