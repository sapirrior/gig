#include "lexer.h"
#include <string.h>
#include <ctype.h>

gig_block_type_t gig_lexer_identify(const char *line) {
    if (!line || *line == '\0' || *line == '\n') return GIG_BLOCK_EMPTY;

    // Strict Triggers
    if (line[0] == '#') {
        if (line[1] == '#') {
            if (line[2] == ' ') return GIG_BLOCK_SUBHEADER;
            return GIG_BLOCK_TEXT; // Unknown/Malformed trigger
        }
        if (line[1] == ' ') return GIG_BLOCK_HEADER;
        return GIG_BLOCK_TEXT; // Malformed
    }

    if (line[0] == '*' && line[1] == ' ') return GIG_BLOCK_BULLET;
    if (line[0] == '$' && line[1] == ' ') return GIG_BLOCK_EXAMPLE;
    if (line[0] == '>' && line[1] == ' ') return GIG_BLOCK_QUOTE;
    if (line[0] == '|' && line[1] == ' ') return GIG_BLOCK_TABLE;
    if (line[0] == '-' && line[1] == ' ') return GIG_BLOCK_DEF;
    if (line[0] == '.' && line[1] == ' ') return GIG_BLOCK_TEXT;

    // If it doesn't start with a trigger and isn't a continuation, it's unknown
    // (Actual identification will happen in parser based on indentation)
    return GIG_BLOCK_TEXT;
}

int gig_lexer_is_continuation(const char *line) {
    if (!line || *line == '\0') return 0;
    return (line[0] == ' ' || line[0] == '\t');
}

int gig_lexer_is_metadata(const char *line) {
    if (!line || *line == '\0') return 0;
    return (line[0] == '@');
}

int gig_lexer_is_separator(const char *line) {
    if (!line || *line == '\0') return 0;
    return (strncmp(line, "---", 3) == 0);
}
