#include "table.h"
#include "utils.h"
#include "wrap.h"
#include "inline.h"
#include "ui/colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Helper to safely append to a buffer with bounds checking */
static void safe_append(char *buf, size_t size, int *pos, const char *fmt, ...) {
    if (*pos >= (int)size - 1) return;
    va_list args;
    va_start(args, fmt);
    int written = vsnprintf(buf + *pos, size - *pos, fmt, args);
    va_end(args);
    if (written > 0) *pos += written;
}

static void draw_border(gig_layout_t *layout, int *widths, int cols, int gutter, int indent, const char *left, const char *mid, const char *right) {
    char buf[4096];
    int pos = 0;
    
    safe_append(buf, sizeof(buf), &pos, "%*s%s%s", gutter + indent, "", GIG_CLR_GREY, left);
    
    for (int i = 0; i < cols; i++) {
        int pad = (widths[i] > 2) ? 1 : 0;
        for (int j = 0; j < widths[i] + (pad * 2); j++) {
            safe_append(buf, sizeof(buf), &pos, "\xe2\x94\x80"); // ─
        }
        if (i < cols - 1) safe_append(buf, sizeof(buf), &pos, "%s", mid);
    }
    
    safe_append(buf, sizeof(buf), &pos, "%s%s", right, GIG_CLR_RESET);
    
    void gig_layout_add_line(gig_layout_t *layout, const char *line);
    gig_layout_add_line(layout, buf);
}

void gig_render_table(gig_layout_t *layout, gig_block_t **curr_ptr, int content_w, int gutter, int indent) {
    gig_block_t *start_row = *curr_ptr;
    gig_block_t *temp = start_row;
    int max_cols = 0;

    // Phase 1: Count rows and max columns
    while (temp && temp->type == GIG_BLOCK_TABLE) {
        int cells = gig_count_cells(temp->content);
        if (cells > max_cols) max_cols = cells;
        temp = temp->next;
    }
    if (max_cols == 0) return;

    // Responsive Stacking (Phase 3 preview - refined)
    // If average col width is too small, stack.
    int min_readable_w = 8;
    if (content_w < (max_cols * min_readable_w) && max_cols > 1) {
        temp = start_row;
        while (temp && temp->type == GIG_BLOCK_TABLE) {
            char *copy = strdup(temp->content);
            char *p = copy;
            for (int i = 0; i < max_cols; i++) {
                char *cell = gig_get_next_cell(&p);
                if (cell && strlen(cell) > 0) {
                    char *proc = gig_inline_process(cell);
                    if (i == 0) {
                        char buf[4096];
                        snprintf(buf, sizeof(buf), "%*s%s•%s %s", gutter + indent, "", GIG_CLR_GREY, GIG_CLR_RESET, proc);
                        void gig_layout_add_line(gig_layout_t *layout, const char *line);
                        gig_layout_add_line(layout, buf);
                    } else {
                        gig_wrap_text(layout, proc, content_w - (indent + 4), gutter + indent + 4);
                    }
                    free(proc);
                }
            }
            free(copy);
            temp = temp->next;
        }
        *curr_ptr = temp;
        return;
    }

    // Measurement and Fluidity detection
    int *col_widths = calloc(max_cols, sizeof(int));
    int *is_fluid = calloc(max_cols, sizeof(int));
    
    temp = start_row;
    while (temp && temp->type == GIG_BLOCK_TABLE) {
        char *line_copy = strdup(temp->content);
        char *p = line_copy;
        for (int col = 0; col < max_cols; col++) {
            char *cell = gig_get_next_cell(&p);
            if (!cell) break;
            
            // Heuristic for fluidity: multiple spaces or single very long word
            int spaces = 0;
            for (int i = 0; cell[i]; i++) if (isspace((unsigned char)cell[i])) spaces++;
            if (spaces > 0 || strlen(cell) > 20) is_fluid[col] = 1;

            char *proc = gig_inline_process(cell);
            int vis_len = gig_get_vis_len(proc);
            if (vis_len > col_widths[col]) col_widths[col] = vis_len;
            free(proc);
        }
        free(line_copy);
        temp = temp->next;
    }

    // Width allocation
    int borders_w = (max_cols + 1) + (max_cols * 2); // | + spaces
    int avail_w = content_w - indent - borders_w;
    if (avail_w < max_cols * 4) avail_w = max_cols * 4;

    int total_req = 0;
    for (int i = 0; i < max_cols; i++) total_req += col_widths[i];

    if (total_req > avail_w) {
        int *final_widths = calloc(max_cols, sizeof(int));
        int fluid_req = 0;
        int fixed_w = 0;
        int fluid_count = 0;

        for (int i = 0; i < max_cols; i++) {
            if (is_fluid[i]) {
                fluid_req += col_widths[i];
                fluid_count++;
            } else {
                fixed_w += col_widths[i];
            }
        }

        // Weighted Elasticity: Distribute remaining space based on content ratio
        int remaining_w = avail_w - fixed_w;
        if (remaining_w < fluid_count * 6) {
            // Extreme squeeze: discard heuristics, everyone gets a fair share of avail_w
            for (int i = 0; i < max_cols; i++) {
                final_widths[i] = avail_w / max_cols;
                if (final_widths[i] < 4) final_widths[i] = 4;
            }
        } else {
            for (int i = 0; i < max_cols; i++) {
                if (!is_fluid[i]) {
                    final_widths[i] = col_widths[i];
                } else {
                    double ratio = (double)col_widths[i] / (fluid_req ? fluid_req : 1);
                    final_widths[i] = (int)(ratio * remaining_w);
                    if (final_widths[i] < 6) final_widths[i] = 6;
                }
            }
        }
        memcpy(col_widths, final_widths, max_cols * sizeof(int));
        free(final_widths);
    }

    // Rendering
    draw_border(layout, col_widths, max_cols, gutter, indent, "\xe2\x94\x8c", "\xe2\x94\xac", "\xe2\x94\x90");

    temp = start_row;
    while (temp && temp->type == GIG_BLOCK_TABLE) {
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
            char buf[4096];
            int pos = 0;
            safe_append(buf, sizeof(buf), &pos, "%*s%s\xe2\x94\x82%s", gutter + indent, "", GIG_CLR_GREY, GIG_CLR_RESET);
            
            for (int col = 0; col < max_cols; col++) {
                int pad = (col_widths[col] > 2) ? 1 : 0;
                if (pad) safe_append(buf, sizeof(buf), &pos, " ");
                
                int cell_vis = 0;
                const char *cell_line = NULL;
                if (h < cell_layouts[col]->count) {
                    cell_line = cell_layouts[col]->lines[h];
                    cell_vis = gig_get_vis_len(cell_line);
                }

                if (temp == start_row) {
                    // Header: Center Align
                    int total_free = col_widths[col] - cell_vis;
                    int pad_left = total_free / 2;
                    int pad_right = total_free - pad_left;
                    
                    for (int i = 0; i < pad_left; i++) if (pos < (int)sizeof(buf) - 1) buf[pos++] = ' ';
                    safe_append(buf, sizeof(buf), &pos, "%s%s%s", GIG_CLR_HEADER, cell_line ? cell_line : "", GIG_CLR_RESET);
                    for (int i = 0; i < pad_right; i++) if (pos < (int)sizeof(buf) - 1) buf[pos++] = ' ';
                } else {
                    // Content: Left Align
                    safe_append(buf, sizeof(buf), &pos, "%s", cell_line ? cell_line : "");
                    for (int i = 0; i < col_widths[col] - cell_vis; i++) {
                        if (pos < (int)sizeof(buf) - 1) buf[pos++] = ' ';
                    }
                }

                if (pad) safe_append(buf, sizeof(buf), &pos, " ");
                safe_append(buf, sizeof(buf), &pos, "%s\xe2\x94\x82%s", GIG_CLR_GREY, GIG_CLR_RESET);
            }
            buf[pos] = '\0';
            void gig_layout_add_line(gig_layout_t *layout, const char *line);
            gig_layout_add_line(layout, buf);
        }

        for (int col = 0; col < max_cols; col++) gig_layout_free(cell_layouts[col]);
        free(cell_layouts);

        if (temp->next && temp->next->type == GIG_BLOCK_TABLE) {
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
