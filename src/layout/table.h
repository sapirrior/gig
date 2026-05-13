#ifndef GIG_LAYOUT_TABLE_H
#define GIG_LAYOUT_TABLE_H

#include "layout.h"
#include "parser/gig.h"

void gig_render_table(gig_layout_t *layout, gig_block_t **curr_ptr, int content_w, int gutter);

#endif
