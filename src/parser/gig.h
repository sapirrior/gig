#ifndef GIG_H
#define GIG_H

#include <stdio.h>
#include <stdlib.h>

/* --- Semantic Block Types --- */
typedef enum {
    GIG_BLOCK_TEXT,
    GIG_BLOCK_HEADER,    // #
    GIG_BLOCK_SUBHEADER, // ##
    GIG_BLOCK_DEF,       // - Term : Desc
    GIG_BLOCK_BULLET,    // *
    GIG_BLOCK_EXAMPLE,   // $
    GIG_BLOCK_QUOTE,     // >
    GIG_BLOCK_TABLE,     // |
    GIG_BLOCK_EMPTY      // Spacing
} gig_block_type_t;

/* --- Metadata Entry --- */
typedef struct {
    char *key;
    char *value;
} gig_meta_t;

/* --- Semantic Block --- */
typedef struct gig_block {
    gig_block_type_t type;
    char *content;         // Main text for headers/paragraphs
    char *term;            // For GIG_BLOCK_DEF (the flag/term)
    struct gig_block *next;
} gig_block_t;

/* --- The Document Object --- */
typedef struct {
    int line_num;
    char *message;
} gig_error_t;

typedef struct {
    gig_meta_t *metadata;
    int meta_count;
    
    gig_block_t *blocks;   // Linked list of semantic blocks
    gig_block_t *last_block;
    
    gig_error_t error;     // Parsing error info (if any)
} gig_doc_t;

/* --- Core Parser API --- */
gig_doc_t* gig_parse_file(const char *path);
void gig_free_doc(gig_doc_t *doc);
const char* gig_get_meta(gig_doc_t *doc, const char *key);

#endif
