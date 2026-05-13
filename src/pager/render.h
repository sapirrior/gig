#ifndef GIG_PAGER_RENDER_H
#define GIG_PAGER_RENDER_H

#include "parser/gig.h"
#include "layout/layout.h"
#include "terminal.h"

void gig_pager_draw_screen(gig_doc_t *doc, gig_layout_t *layout, gig_term_state_t *ts, int scroll_y, const char *search_query);

#endif
