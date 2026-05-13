#ifndef GIG_LEXER_H
#define GIG_LEXER_H

#include "gig.h"

// Identifies the block type based on the first character
gig_block_type_t gig_lexer_identify(const char *line);

// Checks if a line is a continuation (starts with space/tab)
int gig_lexer_is_continuation(const char *line);

// Checks if a line is a metadata entry (@key "value")
int gig_lexer_is_metadata(const char *line);

// Checks if a line is the zone separator (---)
int gig_lexer_is_separator(const char *line);

#endif
