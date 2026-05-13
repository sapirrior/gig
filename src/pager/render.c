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

    // --- Responsive Header Construction ---
    // Priority: Name > Section > Version > Date > Author
    int name_l = (int)strlen(name);
    int sect_l = (int)strlen(section);
    int vers_l = (int)strlen(version);
    int date_l = (int)strlen(date_str);
    int auth_l = author ? (int)strlen(author) + 4 : 0; // " by author"

    int total_vis = sect_l + 3 + name_l + 1 + vers_l; // section / name version
    int show_author = (author && (total_vis + auth_l + date_l + 2 <= ts->cols));
    int show_date = (total_vis + date_l + 2 <= ts->cols);
    int show_version = (sect_l + 3 + name_l + 1 + vers_l <= ts->cols);
    int show_section = (sect_l + 3 + name_l <= ts->cols);

    // Build Left Side
    if (show_section) {
        gig_buffer_printf(&buf, "%s%s%s %s/%s ", GIG_CLR_SUB, section, GIG_CLR_RESET, GIG_CLR_GREY, GIG_CLR_RESET);
    }
    
    // Name (Always show, truncate if extreme)
    if (ts->cols < 10) {
        gig_buffer_append(&buf, name, ts->cols > 0 ? ts->cols : 0);
    } else {
        int avail = ts->cols - (show_section ? sect_l + 3 : 0) - (show_version ? vers_l + 1 : 0) - (show_date ? date_l + 2 : 0) - (show_author ? auth_l : 0);
        if (name_l > avail && avail > 3) {
            gig_buffer_append_str(&buf, GIG_CLR_HEADER);
            gig_buffer_append(&buf, name, avail - 3);
            gig_buffer_append_str(&buf, "...");
            gig_buffer_append_str(&buf, GIG_CLR_RESET);
        } else {
            gig_buffer_printf(&buf, "%s%s%s", GIG_CLR_HEADER, name, GIG_CLR_RESET);
        }
    }

    if (show_version) gig_buffer_printf(&buf, " %s%s%s", GIG_CLR_GREY, version, GIG_CLR_RESET);
    if (show_author) gig_buffer_printf(&buf, " %sby %s%s", GIG_CLR_GREY, author, GIG_CLR_RESET);

    // Right Side (Date)
    int current_vis = (show_section ? sect_l + 3 : 0) + (name_l) + (show_version ? vers_l + 1 : 0) + (show_author ? auth_l : 0);
    if (current_vis > ts->cols) current_vis = ts->cols;

    if (show_date) {
        int spaces = ts->cols - current_vis - date_l;
        for (int i = 0; i < spaces; i++) gig_buffer_append_str(&buf, " ");
        gig_buffer_printf(&buf, "%s%s%s", GIG_CLR_GREY, date_str, GIG_CLR_RESET);
    }
    
    gig_buffer_append_str(&buf, "\x1b[K\r\n\x1b[K\r\n");

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
    
    // --- Responsive Status Bar Construction ---
    int progress = 0;
    if (layout->count > viewport_h) {
        progress = (scroll_y * 100) / (layout->count - viewport_h);
        if (progress > 100) progress = 100;
    } else {
        progress = 100;
    }

    char left[256];
    if (search_query && search_query[0]) {
        snprintf(left, sizeof(left), " GIG /%s ", search_query);
    } else {
        snprintf(left, sizeof(left), " GIG ");
    }
    int left_l = (int)strlen(left);

    // Decide what to show
    int show_bar = (ts->cols > left_l + 25);
    int show_pct = (ts->cols > left_l + 10);

    gig_buffer_append_str(&buf, GIG_CLR_STATUS);
    
    // Handle extreme small width for label
    if (left_l > ts->cols) {
        gig_buffer_append(&buf, left, ts->cols);
    } else {
        gig_buffer_append_str(&buf, left);
    }
    
    int right_len = 0;
    char right_buf[128] = "";
    if (show_pct) {
        if (show_bar) {
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
            right_len = snprintf(right_buf, sizeof(right_buf), " %3d%% %s ", progress, bar);
            // Re-calculate actual visual length because of multi-byte Unicode
            right_len = 5 + 1 + 10 + 2; // " 100% " + 10 chars + " "
        } else {
            right_len = snprintf(right_buf, sizeof(right_buf), " %3d%% ", progress);
        }
    }

    int mid_spaces = ts->cols - left_l - right_len;
    if (mid_spaces < 0) mid_spaces = 0;
    for (int i = 0; i < mid_spaces; i++) gig_buffer_append_str(&buf, " ");
    
    if (right_len > 0) gig_buffer_append_str(&buf, right_buf);
    gig_buffer_append_str(&buf, GIG_CLR_RESET "\x1b[K");

    gig_buffer_flush(&buf);
    gig_buffer_free(&buf);
}
