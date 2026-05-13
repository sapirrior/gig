#include "layout.h"
#include "wrap.h"
#include "inline.h"
#include "table.h"
#include "ui/colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

gig_layout_t* gig_layout_build(gig_doc_t *doc, int screen_width) {
    if (!doc) return NULL;

    gig_layout_t *layout = calloc(1, sizeof(gig_layout_t));
    
    int content_w;
    int gutter;

    // Golden Responsive Container Logic
    if (screen_width <= 40) {
        content_w = screen_width - 2;
        gutter = 1;
    } else if (screen_width <= 84) {
        content_w = screen_width - 8;
        gutter = 4;
    } else {
        content_w = (int)(screen_width * 0.80);
        gutter = (screen_width - content_w) / 2;
    }

    // Proportional Indents (The Staircase)
    int indent_text = (int)(content_w * 0.10);
    int indent_deep = (int)(content_w * 0.15);
    
    // Ensure minimum indents for legibility
    if (indent_text < 4) indent_text = 4;
    if (indent_deep < 6) indent_deep = 6;

    gig_block_t *curr = doc->blocks;
    int first_block = 1;

    void gig_layout_add_line(gig_layout_t *layout, const char *line);

    while (curr) {
        gig_block_type_t type = curr->type;
        
        switch (type) {
            case GIG_BLOCK_TABLE: {
                gig_render_table(layout, &curr, content_w, gutter, indent_text);
                continue; 
            }

            case GIG_BLOCK_EMPTY:
                gig_layout_add_line(layout, "");
                break;

            case GIG_BLOCK_HEADER: {
                if (!first_block) gig_layout_add_line(layout, "");
                char buf[2048];
                snprintf(buf, sizeof(buf), "%*s%s%s%s", gutter, "", GIG_CLR_HEADER, curr->content, GIG_CLR_RESET);
                gig_layout_add_line(layout, buf);
                break;
            }

            case GIG_BLOCK_SUBHEADER: {
                if (!first_block) gig_layout_add_line(layout, "");
                char buf[2048];
                snprintf(buf, sizeof(buf), "%*s%s%s%s", gutter + (indent_text / 2), "", GIG_CLR_SUB, curr->content, GIG_CLR_RESET);
                gig_layout_add_line(layout, buf);
                break;
            }

            case GIG_BLOCK_DEF: {
                char *proc_term = gig_inline_process(curr->term ? curr->term : "");
                char *proc_desc = gig_inline_process(curr->content ? curr->content : "");
                
                if (curr->term && strlen(curr->term) > 0) {
                    char buf[4096];
                    snprintf(buf, sizeof(buf), "%*s%s", gutter + indent_text, "", proc_term);
                    gig_layout_add_line(layout, buf);
                }
                
                if (curr->content && strlen(curr->content) > 0) {
                    gig_wrap_text(layout, proc_desc, content_w - indent_deep, gutter + indent_deep);
                }
                
                free(proc_term);
                free(proc_desc);
                break;
            }

            case GIG_BLOCK_EXAMPLE: {
                char *proc = gig_inline_process(curr->content);
                gig_wrap_text(layout, proc, content_w - indent_deep, gutter + indent_deep);
                free(proc);
                break;
            }

            case GIG_BLOCK_LITERAL: {
                char buf[4096];
                int max_len = content_w - indent_deep;
                int actual_len = strlen(curr->content);

                if (actual_len > max_len && max_len > 3) {
                    char truncated[4096];
                    strncpy(truncated, curr->content, max_len - 3);
                    truncated[max_len - 3] = '\0';
                    strcat(truncated, "...");
                    snprintf(buf, sizeof(buf), "%*s%s%s%s", gutter + indent_deep, "", GIG_CLR_LITERAL, truncated, GIG_CLR_RESET);
                } else {
                    snprintf(buf, sizeof(buf), "%*s%s%s%s", gutter + indent_deep, "", GIG_CLR_LITERAL, curr->content, GIG_CLR_RESET);
                }
                gig_layout_add_line(layout, buf);
                break;
            }

            case GIG_BLOCK_TEXT:
            case GIG_BLOCK_BULLET:
            case GIG_BLOCK_QUOTE: {
                char *proc = gig_inline_process(curr->content);
                int indent = (curr->type == GIG_BLOCK_TEXT) ? indent_text : indent_deep;
                
                if (curr->type == GIG_BLOCK_BULLET) {
                    char *tmp = malloc(strlen(proc) + 64);
                    sprintf(tmp, "%s•%s %s", GIG_CLR_GREY, GIG_CLR_RESET, proc);
                    free(proc);
                    proc = tmp;
                } else if (curr->type == GIG_BLOCK_QUOTE) {
                    char *tmp = malloc(strlen(proc) + 64);
                    sprintf(tmp, "%s%s%s", GIG_CLR_QUOTE, proc, GIG_CLR_RESET);
                    free(proc);
                    proc = tmp;
                }

                gig_wrap_text(layout, proc, content_w - indent, gutter + indent);
                free(proc);
                break;
            }
        }
        first_block = 0;
        curr = curr->next;
    }

    gig_layout_add_line(layout, ""); 
    return layout;
}
