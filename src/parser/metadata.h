#ifndef GIG_METADATA_H
#define GIG_METADATA_H

#include "gig.h"

// Parses @key "value" into a gig_meta_t entry
int gig_parse_meta_line(const char *line, gig_doc_t *doc);

#endif
