#include "gig.h"
#include "lexer.h"
#include "metadata.h"
#include "doc.h"
#include <string.h>
#include <ctype.h>

gig_doc_t* gig_parse_file(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) return NULL;

    gig_doc_t *doc = calloc(1, sizeof(gig_doc_t));
    char line[4096];
    int line_num = 0;
    int in_body = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') line[--len] = '\0';
        if (len > 0 && line[len-1] == '\r') line[--len] = '\0';

        if (!in_body) {
            if (gig_lexer_is_separator(line)) {
                in_body = 1;
                if (!gig_get_meta(doc, "name") || !gig_get_meta(doc, "section")) {
                    gig_set_error(doc, line_num, "Missing required metadata: '@name' or '@section'");
                    break;
                }
                continue;
            }
            if (gig_lexer_is_metadata(line)) {
                int res = gig_parse_meta_line(line, doc);
                if (res == 0) {
                    gig_set_error(doc, line_num, "Invalid metadata format. Use: @key \"value\"");
                    break;
                } else if (res == -1) {
                    gig_set_error(doc, line_num, "Unauthorized metadata key. Forbidden.");
                    break;
                }
            } else if (strlen(line) > 0) {
                gig_set_error(doc, line_num, "Only metadata (@) or separator (---) allowed above content.");
                break;
            }
            continue;
        }

        if (gig_lexer_is_continuation(line)) {
            if (!doc->last_block || doc->last_block->type == GIG_BLOCK_EMPTY) {
                gig_set_error(doc, line_num, "Unexpected continuation. No block to append to.");
                break;
            }
            gig_append_to_last_block(doc, line);
        } else {
            gig_block_type_t type = gig_lexer_identify(line);
            const char *content;
            char *term = NULL;

            if (type == GIG_BLOCK_EMPTY) {
                gig_add_block(doc, type, "", NULL);
                continue;
            }

            if (type == GIG_BLOCK_HEADER) content = line + 2;
            else if (type == GIG_BLOCK_SUBHEADER) content = line + 3;
            else if (type == GIG_BLOCK_BULLET) content = line + 2;
            else if (type == GIG_BLOCK_EXAMPLE) content = line + 2;
            else if (type == GIG_BLOCK_QUOTE) content = line + 2;
            else if (type == GIG_BLOCK_TABLE) content = line + 2;
            else if (type == GIG_BLOCK_TEXT) content = line + 2;
            else if (type == GIG_BLOCK_DEF) {
                const char *sep = strchr(line + 1, ':');
                if (!sep) {
                    gig_set_error(doc, line_num, "Definition formation error. Missing ':' separator.");
                    break;
                }
                size_t t_len = sep - (line + 1);
                term = malloc(t_len + 1);
                memcpy(term, line + 1, t_len);
                term[t_len] = '\0';
                content = sep + 1;
            } else {
                gig_set_error(doc, line_num, "Unknown block formation. Every line must start with a trigger (#, ##, *, -, $, >) or space.");
                break;
            }

            gig_add_block(doc, type, content, term);
            if (term) free(term);
        }
    }

    fclose(fp);
    return doc;
}

void gig_free_doc(gig_doc_t *doc) {
    if (!doc) return;
    for (int i = 0; i < doc->meta_count; i++) {
        free(doc->metadata[i].key);
        free(doc->metadata[i].value);
    }
    free(doc->metadata);

    gig_block_t *curr = doc->blocks;
    while (curr) {
        gig_block_t *next = curr->next;
        free(curr->content);
        free(curr->term);
        free(curr);
        curr = next;
    }
    if (doc->error.message) free(doc->error.message);
    free(doc);
}
