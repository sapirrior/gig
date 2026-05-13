#ifndef GIG_PAGER_INPUT_H
#define GIG_PAGER_INPUT_H

#include "terminal.h"

int gig_pager_read_key();
void gig_pager_prompt(gig_term_state_t *ts, const char *prompt, char *buf, int size);

#endif
