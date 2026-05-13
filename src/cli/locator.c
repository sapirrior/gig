#include "locator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#ifdef __TERMUX__
#define GIG_PATH "/data/data/com.termux/files/usr/share/gig"
#else
#define GIG_PATH "/usr/share/gig"
#endif

const char* gig_get_path_string(void) {
    return GIG_PATH;
}

char* gig_locate_page(const char *section, const char *name) {
    if (!name) return NULL;

    // Check if name is already a direct path
    struct stat st;
    if (stat(name, &st) == 0) return strdup(name);

    char path[1024];

    if (section) {
        snprintf(path, sizeof(path), "%s/%s/%s.gg", GIG_PATH, section, name);
        if (stat(path, &st) == 0) return strdup(path);
    } else {
        // Standard Manual Search Order: 
        // 1 (Cmds), n (New), l (Local), 8 (Admin), 3 (Lib), 2 (Sys), 
        // 5 (Formats), 4 (Devices), 9 (Kernel), 6 (Games), 7 (Misc)
        const char *sections[] = {"1", "n", "l", "8", "3", "2", "5", "4", "9", "6", "7", NULL};
        
        for (int i = 0; sections[i]; i++) {
            snprintf(path, sizeof(path), "%s/%s/%s.gg", GIG_PATH, sections[i], name);
            if (stat(path, &st) == 0) return strdup(path);
        }
    }

    return NULL;
}
