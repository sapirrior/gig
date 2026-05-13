#ifndef GIG_LOCATOR_H
#define GIG_LOCATOR_H

// Returns the hardcoded system GIG path
const char* gig_get_path_string(void);

// Finds a .gg file in the standard system path
// Returns heap-allocated path or NULL
char* gig_locate_page(const char *section, const char *name);

// Returns 1 if the string is a valid GIG section, 0 otherwise
int gig_is_valid_section(const char *s);

#endif
