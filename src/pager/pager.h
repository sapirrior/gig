#ifndef GIG_PAGER_H
#define GIG_PAGER_H

#include "layout/layout.h"
#include "terminal.h"

// Entry point for the interactive pager
void gig_pager_run(gig_doc_t *doc, gig_layout_t *layout);

// Internal loop for the pager (assumes terminal is already setup)
void gig_pager_loop(gig_doc_t *doc, gig_layout_t *layout, gig_term_state_t *ts);

#endif
