#define _GNU_SOURCE
#include "render.h"
#include "buffer.h"
#include "ui/colors.h"
#include "parser/metadata.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

static void append_with_highlight(gig_buffer_t *buf, const char *line, const char *query) {
    if (!query || !query[0]) {
        gig_buffer_append_str(buf, line);
        return;
    }

    const char *p = line;
    int query_len = (int)strlen(query);

    while (*p) {
        // Handle ANSI sequences first (skip highlighting them)
        if (*p == '\x1b') {
            const char *start = p;
            while (*p && *p != 'm') p++;
            if (*p) p++;
            gig_buffer_append(buf, start, p - start);
            continue;
        }

        // Search for the next match of the query
        const char *match = strcasestr(p, query);
        if (match == p) {
            // Match found at current position: wrap in search color
            gig_buffer_append_str(buf, GIG_CLR_SEARCH);
            
            // Advance through the match, preserving any embedded ANSI sequences
            int matched_bytes = 0;
            while (matched_bytes < query_len && *p) {
                if (*p == '\x1b') {
                    const char *seq_start = p;
                    while (*p && *p != 'm') p++;
                    if (*p) p++;
                    gig_buffer_append(buf, seq_start, p - seq_start);
                } else {
                    gig_buffer_append(buf, p++, 1);
                    matched_bytes++;
                }
            }
            gig_buffer_append_str(buf, GIG_CLR_RESET);
        } else {
            // No match at current position: append characters until next match or ANSI or end
            const char *next_special = p;
            while (*next_special && *next_special != '\x1b' && next_special != match) {
                if (match && next_special == match) break;
                next_special++;
            }
            
            if (next_special > p) {
                gig_buffer_append(buf, p, next_special - p);
                p = next_special;
            } else if (*p) {
                // Should not happen with logic above, but safety first
                gig_buffer_append(buf, p++, 1);
            }
        }
    }
}

void gig_pager_draw_screen(gig_doc_t *doc, gig_layout_t *layout, gig_term_state_t *ts, int scroll_y, const char *search_query) {
    gig_buffer_t buf;
    gig_buffer_init(&buf);

    gig_buffer_append_str(&buf, "\x1b[H");

    const char *name = gig_get_meta(doc, "name");
    const char *section = gig_get_meta(doc, "section");
    const char *version = gig_get_meta(doc, "version");
    const char *author = gig_get_meta(doc, "author");
    if (!name) name = "Unknown";
    if (!section) section = "?";
    if (!version) version = "0.0.0";

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char date_str[64];
    strftime(date_str, sizeof(date_str), "%Y-%m-%d", t);

    char left_part[2048];
    snprintf(left_part, sizeof(left_part), "%s%s%s %s/%s %s%s%s %s%s%s",
        GIG_CLR_SUB, section, GIG_CLR_RESET,
        GIG_CLR_GREY, GIG_CLR_RESET,
        GIG_CLR_HEADER, name, GIG_CLR_RESET,
        GIG_CLR_GREY, version, GIG_CLR_RESET);
    
    int vis_len = (int)strlen(section) + (int)strlen(name) + (int)strlen(version) + 4;
    if (author) {
        char auth_buf[1024];
        snprintf(auth_buf, sizeof(auth_buf), " %sby %s%s", GIG_CLR_GREY, author, GIG_CLR_RESET);
        if (strlen(left_part) + strlen(auth_buf) < sizeof(left_part)) {
            strcat(left_part, auth_buf);
        }
        vis_len += (int)strlen(author) + 4;
    }

    int date_vis = (int)strlen(date_str);
    int head_spaces = ts->cols - vis_len - date_vis;
    if (head_spaces < 0) head_spaces = 0;

    gig_buffer_append_str(&buf, left_part);
    for (int i = 0; i < head_spaces; i++) gig_buffer_append_str(&buf, " ");
    gig_buffer_printf(&buf, "%s%s%s\x1b[K\r\n", GIG_CLR_GREY, date_str, GIG_CLR_RESET);
    
    gig_buffer_append_str(&buf, "\x1b[K\r\n");

    int header_h = 2;
    int footer_h = 1;
    int bottom_pad = 1; 
    int viewport_h = ts->rows - header_h - footer_h - bottom_pad;

    for (int i = 0; i < viewport_h; i++) {
        int idx = scroll_y + i;
        if (idx < layout->count) {
            append_with_highlight(&buf, layout->lines[idx], search_query);
        }
        gig_buffer_append_str(&buf, "\x1b[K\r\n");
    }

    gig_buffer_append_str(&buf, "\x1b[K\r\n");

    gig_terminal_move_cursor(ts->rows, 1);
    
    int progress = 0;
    if (layout->count > viewport_h) {
        progress = (scroll_y * 100) / (layout->count - viewport_h);
        if (progress > 100) progress = 100;
    } else {
        progress = 100;
    }

    char bar[64];
    int bar_width = 10;
    int filled = (progress * bar_width) / 100;
    int b_idx = 0;
    for (int i = 0; i < bar_width; i++) {
        if (i < filled) b_idx += sprintf(bar + b_idx, "━");
        else if (i == filled) b_idx += sprintf(bar + b_idx, "╸");
        else b_idx += sprintf(bar + b_idx, "─");
    }
    bar[b_idx] = '\0';

    char left[256], right[256];
    if (search_query && search_query[0]) {
        snprintf(left, sizeof(left), " GIG /%s ", search_query);
    } else {
        snprintf(left, sizeof(left), " GIG ");
    }
    snprintf(right, sizeof(right), " %3d%% %s ", progress, bar);

    gig_buffer_append_str(&buf, GIG_CLR_STATUS);
    gig_buffer_append_str(&buf, left);
    
    int mid_spaces = ts->cols - (int)strlen(left) - 18; 
    if (mid_spaces < 0) mid_spaces = 0;
    for (int i = 0; i < mid_spaces; i++) gig_buffer_append_str(&buf, " ");
    
    gig_buffer_append_str(&buf, right);
    gig_buffer_append_str(&buf, GIG_CLR_RESET "\x1b[K");

    gig_buffer_flush(&buf);
    gig_buffer_free(&buf);
}
