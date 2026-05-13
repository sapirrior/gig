#include "doc.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char *trim_and_copy(const char *src) {
    if (!src) return NULL;
    while (*src && isspace((unsigned char)*src)) src++;
    size_t len = strlen(src);
    while (len > 0 && isspace((unsigned char)src[len-1])) len--;
    char *res = malloc(len + 1);
    if (res) {
        memcpy(res, src, len);
        res[len] = '\0';
    }
    return res;
}

void gig_set_error(gig_doc_t *doc, int line, const char *msg) {
    if (doc->error.message) return; 
    doc->error.line_num = line;
    doc->error.message = strdup(msg);
}

void gig_add_block(gig_doc_t *doc, gig_block_type_t type, const char *content, const char *term) {
    gig_block_t *block = calloc(1, sizeof(gig_block_t));
    block->type = type;
    block->content = trim_and_copy(content);
    block->term = trim_and_copy(term);

    if (!doc->blocks) {
        doc->blocks = block;
    } else {
        doc->last_block->next = block;
    }
    doc->last_block = block;
}

void gig_append_to_last_block(gig_doc_t *doc, const char *extra) {
    if (!doc->last_block || !extra) return;
    
    char *trimmed = trim_and_copy(extra);
    if (!trimmed) return;

    size_t old_len = doc->last_block->content ? strlen(doc->last_block->content) : 0;
    size_t extra_len = strlen(trimmed);
    
    char *new_content = malloc(old_len + extra_len + 2);
    if (new_content) {
        if (doc->last_block->content) {
            memcpy(new_content, doc->last_block->content, old_len);
            new_content[old_len] = ' ';
            memcpy(new_content + old_len + 1, trimmed, extra_len);
            new_content[old_len + extra_len + 1] = '\0';
            free(doc->last_block->content);
        } else {
            strcpy(new_content, trimmed);
        }
        doc->last_block->content = new_content;
    }
    free(trimmed);
}
