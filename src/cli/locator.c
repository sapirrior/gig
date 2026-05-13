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

    // Check if name is already a direct path (like "file.gg")
    struct stat st;
    if (stat(name, &st) == 0) return strdup(name);

    char path[1024];
    const char *target_section = section ? section : "1";

    snprintf(path, sizeof(path), "%s/%s/%s.gg", GIG_PATH, target_section, name);
    if (stat(path, &st) == 0) return strdup(path);

    return NULL;
}

int gig_is_valid_section(const char *s) {
    if (!s || strlen(s) != 1) return 0;
    const char *valid = "123456789nl";
    return strchr(valid, s[0]) != NULL;
}
