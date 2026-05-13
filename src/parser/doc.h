#ifndef GIG_PARSER_DOC_H
#define GIG_PARSER_DOC_H

#include "gig.h"

void gig_add_block(gig_doc_t *doc, gig_block_type_t type, const char *content, const char *term);
void gig_append_to_last_block(gig_doc_t *doc, const char *extra);
void gig_set_error(gig_doc_t *doc, int line, const char *msg);

#endif
