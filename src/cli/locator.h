#ifndef GIG_LOCATOR_H
#define GIG_LOCATOR_H

// Returns the hardcoded system GIG path
const char* gig_get_path_string(void);

// Finds a .gg file in the standard system path
// Returns heap-allocated path or NULL
char* gig_locate_page(const char *section, const char *name);

#endif
