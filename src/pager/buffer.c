#include "buffer.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

void gig_buffer_init(gig_buffer_t *buf) {
    buf->cap = 16384;
    buf->data = malloc(buf->cap);
    buf->len = 0;
}

void gig_buffer_append(gig_buffer_t *buf, const char *s, size_t n) {
    if (buf->len + n >= buf->cap) {
        buf->cap *= 2;
        buf->data = realloc(buf->data, buf->cap);
    }
    memcpy(buf->data + buf->len, s, n);
    buf->len += n;
}

void gig_buffer_append_str(gig_buffer_t *buf, const char *s) {
    gig_buffer_append(buf, s, strlen(s));
}

void gig_buffer_printf(gig_buffer_t *buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int needed = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (needed < 0) return;
    
    if (buf->len + (size_t)needed >= buf->cap) {
        buf->cap = buf->len + needed + 1024;
        buf->data = realloc(buf->data, buf->cap);
    }

    va_start(args, fmt);
    vsnprintf(buf->data + buf->len, (size_t)needed + 1, fmt, args);
    va_end(args);
    buf->len += (size_t)needed;
}

void gig_buffer_flush(gig_buffer_t *buf) {
    write(STDOUT_FILENO, buf->data, buf->len);
    buf->len = 0;
}

void gig_buffer_free(gig_buffer_t *buf) {
    free(buf->data);
}
