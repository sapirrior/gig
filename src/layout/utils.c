#include "utils.h"
#include <string.h>

int gig_get_vis_len(const char *s) {
    int vis = 0;
    const char *p = s;
    while (*p) {
        if (*p == '\x1b') {
            while (*p && *p != 'm') p++;
            if (*p) p++;
        } else {
            if ((*p & 0xc0) != 0x80) vis++;
            p++;
        }
    }
    return vis;
}

char* gig_get_next_cell(char **s) {
    if (!*s) return NULL;
    char *start = *s;
    char *end = strchr(start, '|');
    if (end) {
        *end = '\0';
        *s = end + 1;
    } else {
        *s = NULL;
    }
    return start;
}

int gig_count_cells(const char *s) {
    if (!s) return 0;
    int count = 1;
    for (const char *p = s; *p; p++) if (*p == '|') count++;
    return count;
}
