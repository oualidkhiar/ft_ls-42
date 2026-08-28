#include "../include/display.h"

static size_t count_total_blocks_KB(t_metadata *base, size_t size, unsigned int *link_width, unsigned int *size_width)
{
    size_t total_block_512B = 0;
    unsigned int link_w = 0;
    unsigned int size_w = 0;

    for (size_t i = 0; i < size; i++) {

        struct stat sb = base[i].st;

        total_block_512B += sb.st_blocks;

        if (link_w < sb.st_nlink) 
            link_w = sb.st_nlink;
        if (size_w < sb.st_size)
            size_w = sb.st_size;
    }

    *link_width = integer_len(link_w);
    *size_width = integer_len(size_w);

    return (total_block_512B / 2);
}


static void print_symbolic_permissions(struct stat sb)
{
    printf(
            (S_ISDIR(sb.st_mode))  ? "d" : 
            (S_ISLNK(sb.st_mode)) ? "l" : "-"
        );

    // User permissions
    printf((sb.st_mode & S_IRUSR) ? "r" : "-");
    printf((sb.st_mode & S_IWUSR) ? "w" : "-");
    printf((sb.st_mode & S_IXUSR) ? "x" : "-");

    // Group permissions
    printf((sb.st_mode & S_IRGRP) ? "r" : "-");
    printf((sb.st_mode & S_IWGRP) ? "w" : "-");
    printf((sb.st_mode & S_IXGRP) ? "x" : "-");

    // Other permissions
    printf((sb.st_mode & S_IROTH) ? "r" : "-");
    printf((sb.st_mode & S_IWOTH) ? "w" : "-");
    printf((sb.st_mode & S_IXOTH) ? "x " : "- ");
}

static void print_owr_gpr_lnk_size(struct stat sb, unsigned int link_width, unsigned int size_width)
{
    struct passwd *pw;
    struct group  *gr;

    printf("%*lu ", link_width, sb.st_nlink); // priint number of links 

    pw = getpwuid(sb.st_uid);
    if (pw) {
        printf("%s ", pw->pw_name);
    } else {
        printf("%d ", sb.st_uid); // Fallback to UID if name not found
    }

    gr = getgrgid(sb.st_gid);
    if (gr) {
        printf("%s ", gr->gr_name);
    } else {
        printf("%d ", sb.st_gid); // Fallback to GID if name not found
    }

    printf("%*lu ", size_width, sb.st_size);
}


static void print_date(struct stat sb)
{
    char *months[13] = {
                        "" , 
                        "Jan", 
                        "Feb", 
                        "Mar", 
                        "Apr", 
                        "May", 
                        "Jun", 
                        "Jul", 
                        "Aug", 
                        "Sep", 
                        "Oct", 
                        "Nov", 
                        "Dec"
                    };

    struct tm *time_info = localtime(&sb.st_mtime);

    printf(
        "%s %02d %02d:%02d ", 
        months[time_info->tm_mon], 
        time_info->tm_mday, 
        time_info->tm_hour, 
        time_info->tm_min
    );
}

static void print_file_info(struct stat sb, unsigned int link_width, unsigned int size_width)
{
    print_symbolic_permissions(sb);
    print_owr_gpr_lnk_size(sb, link_width, size_width);
    print_date(sb);
}

bool is_executble(struct stat sb)
{
    return (
            (sb.st_mode & S_IXUSR) ||  
            (sb.st_mode & S_IXGRP) ||  
            (sb.st_mode & S_IXOTH)
        );
}

void print_names(t_metadata *base, t_flags *opts, size_t size, int level, bool *printed)
{
    size_t i = 0;
    bool first_line = true;

    unsigned int link_width = 0;
    unsigned int size_width = 0;

    while (i < size)
    {
        if (base[i].name && !(base[i].is_dir && level == 0))
        {
            if (*printed && !(opts->l)) printf(" ");

            if (opts->l) {
                if (first_line) {
                    size_t total_blocks = count_total_blocks_KB(
                                            base, 
                                            size, 
                                            &link_width, 
                                            &size_width
                                        );

                    printf("total %zu\n", total_blocks);
                }
                print_file_info(base[i].st, link_width, size_width);
                first_line = false;
            }

            printf(
                    base[i].is_dir ? BLUE"%s"RESET : 
                    (is_executble(base[i].st)) ? GREEN"%s"RESET : 
                    "%s", base[i].name
                );

            if (opts->l) printf("\n");

            *printed = true;
        }
        i++;
    }
    if (*printed && !(opts->l))
        printf("\n");
}

void print_list(t_metadata *base, t_flags *opts, size_t size, int level, bool first_section, char *parent_level)
{
    bool printed = false;
    bool need_gap;
    size_t i = 0;

    print_names(base, opts, size, level, &printed);

    need_gap = printed || !first_section;

    while (i < size)
    {
        if (base[i].next_depth)
        {
            char *next_level = NULL;

            if (opts->r_upper || size > 1)
            {
                if (need_gap)
                    printf("\n");

                char *current_level = (strjoin(parent_level, base[i].name));

                if (!current_level) {
                    current_level = strdup(base[i].name);
                }
 
                printf("%s:\n", current_level);

                next_level = (current_level[strlen(current_level) - 1] != '/') ? 
                                        strjoin(current_level, "/") : strdup(current_level);
                free(current_level);

                need_gap = true;
            }

            print_list(
                        base[i].next_depth, 
                        opts, 
                        base[i].content_count, 
                        level + 1, 
                        false, 
                        next_level
                    );

            free(next_level);
        }
        i++;
    }
}