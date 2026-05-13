#ifndef GIG_LAYOUT_H
#define GIG_LAYOUT_H

#include "parser/gig.h"

/* --- Rendered Line Structure --- */
typedef struct {
    char **lines;
    int count;
    int capacity;
} gig_layout_t;

/* --- Layout Context --- */
typedef struct {
    int term_width;
    int gutter_width;
    int content_width;
    
    gig_layout_t *rendered;
} gig_layout_ctx_t;

/* --- Public Layout API --- */
gig_layout_t* gig_layout_build(gig_doc_t *doc, int screen_width);
void gig_layout_free(gig_layout_t *layout);

#endif
