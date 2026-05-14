#include "table.h"
#include "utils.h"
#include "wrap.h"
#include "inline.h"
#include "ui/colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static void draw_border(gig_layout_t *layout, int *widths, int cols, int gutter, int indent, const char *left, const char *mid, const char *right) {
    char buf[65536];
    int b_idx = snprintf(buf, sizeof(buf), "%*s%s", gutter + indent, "", GIG_CLR_GREY);
    
    if (b_idx < (int)sizeof(buf) - 64) b_idx += sprintf(buf + b_idx, "%s", left);
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < widths[i] + 2; j++) {
            if (b_idx < (int)sizeof(buf) - 64) b_idx += sprintf(buf + b_idx, "\xe2\x94\x80"); // ─
        }
        if (i < cols - 1) {
            if (b_idx < (int)sizeof(buf) - 64) b_idx += sprintf(buf + b_idx, "%s", mid);
        }
    }
    if (b_idx < (int)sizeof(buf) - 64) sprintf(buf + b_idx, "%s%s", right, GIG_CLR_RESET);
    
    void gig_layout_add_line(gig_layout_t *layout, const char *line);
    gig_layout_add_line(layout, buf);
}

/**
 * Heuristic: Determine if a column is "Fluid" (needs wrapping) or "Fixed" (label).
 */
static int is_column_fluid(gig_block_t *row, int col_idx) {
    int total_spaces = 0;
    int max_len = 0;
    gig_block_t *temp = row;
    while (temp && temp->type == GIG_BLOCK_TABLE) {
        char *copy = strdup(temp->content);
        char *p = copy;
        for (int i = 0; i <= col_idx; i++) {
            char *cell = gig_get_next_cell(&p);
            if (i == col_idx && cell) {
                int len = strlen(cell);
                if (len > max_len) max_len = len;
                for (int j = 0; cell[j]; j++) if (isspace((unsigned char)cell[j])) total_spaces++;
            }
        }
        free(copy);
        temp = temp->next;
    }
    // A column is fluid if it has multiple words or is very long
    return (total_spaces > 1 || max_len > 20);
}

void gig_render_table(gig_layout_t *layout, gig_block_t **curr_ptr, int content_w, int gutter, int indent) {
    gig_block_t *row = *curr_ptr;
    gig_block_t *temp = row;
    int row_count = 0;
    int max_cols = 0;

    while (temp && temp->type == GIG_BLOCK_TABLE) {
        int cells = gig_count_cells(temp->content);
        if (cells > max_cols) max_cols = cells;
        row_count++;
        temp = temp->next;
    }
    if (max_cols == 0) return;

    // --- Responsive Stacking Logic ---
    // If the viewport is too narrow, render as a definition list
    if (content_w < 40 && max_cols > 1) {
        temp = row;
        while (temp && temp->type == GIG_BLOCK_TABLE) {
            char *copy = strdup(temp->content);
            char *p = copy;
            char *key = gig_get_next_cell(&p);
            char *val = gig_get_next_cell(&p);
            
            if (key) {
                char *pk = gig_inline_process(key);
                char buf[4096];
                snprintf(buf, sizeof(buf), "%*s%s•%s %s", gutter + indent, "", GIG_CLR_GREY, GIG_CLR_RESET, pk);
                void gig_layout_add_line(gig_layout_t *layout, const char *line);
                gig_layout_add_line(layout, buf);
                free(pk);
            }
            if (val) {
                char *pv = gig_inline_process(val);
                gig_wrap_text(layout, pv, content_w - (indent + 4), gutter + indent + 4);
                free(pv);
            }
            free(copy);
            temp = temp->next;
        }
        *curr_ptr = temp;
        return;
    }

    // --- Standard Grid Logic with Heuristic Balancing ---
    int *col_widths = calloc(max_cols, sizeof(int));
    int *is_fluid = calloc(max_cols, sizeof(int));
    
    for (int col = 0; col < max_cols; col++) {
        is_fluid[col] = is_column_fluid(row, col);
    }

    temp = row;
    for (int i = 0; i < row_count; i++) {
        char *line_copy = strdup(temp->content);
        char *p = line_copy;
        int col = 0;
        char *cell;
        while ((cell = gig_get_next_cell(&p)) && col < max_cols) {
            char *proc = gig_inline_process(cell);
            int vis_len = gig_get_vis_len(proc);
            if (vis_len > col_widths[col]) col_widths[col] = vis_len;
            free(proc);
            col++;
        }
        free(line_copy);
        temp = temp->next;
    }

    int sep_w = (max_cols + 1) + (max_cols - 1); 
    int avail_w = content_w - indent - sep_w - (max_cols * 2);
    if (avail_w < max_cols * 4) avail_w = max_cols * 4;

    int total_req = 0;
    for (int i = 0; i < max_cols; i++) total_req += col_widths[i];

    if (total_req > avail_w) {
        int *final_widths = calloc(max_cols, sizeof(int));
        int remaining_w = avail_w;
        int fluid_count = 0;
        
        // Step 1: Satisfy Fixed columns first (up to 30% of width each)
        for (int i = 0; i < max_cols; i++) {
            if (!is_fluid[i]) {
                int cap = avail_w / 3;
                final_widths[i] = (col_widths[i] < cap) ? col_widths[i] : cap;
                if (final_widths[i] < 4) final_widths[i] = 4;
                remaining_w -= final_widths[i];
            } else {
                fluid_count++;
            }
        }
        
        // Step 2: Distribute remaining width to Fluid columns
        if (fluid_count > 0) {
            int share = remaining_w / fluid_count;
            for (int i = 0; i < max_cols; i++) {
                if (is_fluid[i]) {
                    final_widths[i] = (share > 4) ? share : 4;
                    remaining_w -= final_widths[i];
                }
            }
        }
        memcpy(col_widths, final_widths, max_cols * sizeof(int));
        free(final_widths);
    }

    // Top Border
    draw_border(layout, col_widths, max_cols, gutter, indent, "\xe2\x94\x8c", "\xe2\x94\xac", "\xe2\x94\x90");

    temp = row;
    for (int r = 0; r < row_count; r++) {
        gig_layout_t **cell_layouts = calloc(max_cols, sizeof(gig_layout_t*));
        int max_h = 0;
        
        char *line_copy = strdup(temp->content);
        char *p_copy = line_copy;
        for (int col = 0; col < max_cols; col++) {
            char *cell = gig_get_next_cell(&p_copy);
            char *proc = gig_inline_process(cell ? cell : "");
            cell_layouts[col] = calloc(1, sizeof(gig_layout_t));
            gig_wrap_text(cell_layouts[col], proc, col_widths[col], 0);
            if (cell_layouts[col]->count > max_h) max_h = cell_layouts[col]->count;
            free(proc);
        }
        free(line_copy);

        for (int h = 0; h < max_h; h++) {
            char buf[65536];
            int b_idx = snprintf(buf, sizeof(buf), "%*s%s\xe2\x94\x82%s", gutter + indent, "", GIG_CLR_GREY, GIG_CLR_RESET);
            
            for (int col = 0; col < max_cols; col++) {
                if (b_idx < (int)sizeof(buf) - 1024) b_idx += sprintf(buf + b_idx, " ");
                if (r == 0 && b_idx < (int)sizeof(buf) - 1024) b_idx += sprintf(buf + b_idx, "%s", GIG_CLR_HEADER);
                
                int cell_vis = 0;
                if (h < cell_layouts[col]->count) {
                    const char *cell_line = cell_layouts[col]->lines[h];
                    cell_vis = gig_get_vis_len(cell_line);
                    if (b_idx + (int)strlen(cell_line) < (int)sizeof(buf) - 1024) {
                        b_idx += sprintf(buf + b_idx, "%s", cell_line);
                    }
                }
                for (int i = 0; i < col_widths[col] - cell_vis; i++) {
                    if (b_idx < (int)sizeof(buf) - 1024) buf[b_idx++] = ' ';
                }
                if (r == 0 && b_idx < (int)sizeof(buf) - 1024) b_idx += sprintf(buf + b_idx, "%s", GIG_CLR_RESET);
                if (b_idx < (int)sizeof(buf) - 1024) b_idx += sprintf(buf + b_idx, " %s\xe2\x94\x82%s", GIG_CLR_GREY, GIG_CLR_RESET);
            }
            buf[b_idx] = '\0';
            void gig_layout_add_line(gig_layout_t *layout, const char *line);
            gig_layout_add_line(layout, buf);
        }

        for (int col = 0; col < max_cols; col++) gig_layout_free(cell_layouts[col]);
        free(cell_layouts);

        if (r < row_count - 1) {
            draw_border(layout, col_widths, max_cols, gutter, indent, "\xe2\x94\x9c", "\xe2\x94\xbc", "\xe2\x94\xa4");
        } else {
            draw_border(layout, col_widths, max_cols, gutter, indent, "\xe2\x94\x94", "\xe2\x94\xb4", "\xe2\x94\x98");
        }
        temp = temp->next;
    }

    free(col_widths);
    free(is_fluid);
    *curr_ptr = temp;
}
