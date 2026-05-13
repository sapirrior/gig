#ifndef GIG_BUFFER_H
#define GIG_BUFFER_H

#include <stdlib.h>

typedef struct {
    char *data;
    size_t len;
    size_t cap;
} gig_buffer_t;

void gig_buffer_init(gig_buffer_t *buf);
void gig_buffer_append(gig_buffer_t *buf, const char *s, size_t n);
void gig_buffer_append_str(gig_buffer_t *buf, const char *s);
void gig_buffer_printf(gig_buffer_t *buf, const char *fmt, ...);
void gig_buffer_flush(gig_buffer_t *buf);
void gig_buffer_free(gig_buffer_t *buf);

#endif
