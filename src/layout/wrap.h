#ifndef GIG_WRAP_H
#define GIG_WRAP_H

#include "layout.h"

// Wraps a semantic string into multiple layout lines
void gig_wrap_text(gig_layout_t *layout, const char *processed_text, int width, int left_pad);

#endif
