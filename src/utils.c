#include "../include/utils.h"
#include "../include/ft_ls_help.h"

bool is_white_space(char c)
{
    return ((c >= 9 && c <= 13) || c == 32);
}

bool not_current_or_parent_dir(const char *path)
{
    return (strcmp(path, ".") != 0 && strcmp(path, "..") != 0);
}

void ft_ls_help()
{
    printf("%s", ls_usage);
}

char *strjoin(const char *s1, const char *s2)
{
    if (!s1 || !s2)
        return NULL;

    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1);

    if (!result)
        return NULL;

    strcpy(result, s1);
    strcat(result, s2);

    return result;
}