#include "table.h"
#include "utils.h"
#include "wrap.h"
#include "inline.h"
#include "ui/colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static void draw_border(gig_layout_t *layout, int *widths, int cols, int gutter, const char *left, const char *mid, const char *right) {
    char buf[65536];
    int b_idx = snprintf(buf, sizeof(buf), "%*s%s", gutter + 7, "", GIG_CLR_GREY);
    
    if (b_idx < (int)sizeof(buf) - 64) b_idx += sprintf(buf + b_idx, "%s", left);
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < widths[i] + 2; j++) {
            if (b_idx < (int)sizeof(buf) - 64) b_idx += sprintf(buf + b_idx, "\xe2\x94\x80"); // ─
        }
        if (i < cols - 1) {
            if (b_idx < (int)sizeof(buf) - 64) b_idx += sprintf(buf + b_idx, "%s", mid);
        }
    }
    if (b_idx < (int)sizeof(buf) - 64) b_idx += sprintf(buf + b_idx, "%s%s", right, GIG_CLR_RESET);
    
    void gig_layout_add_line(gig_layout_t *layout, const char *line);
    gig_layout_add_line(layout, buf);
}

void gig_render_table(gig_layout_t *layout, gig_block_t **curr_ptr, int content_w, int gutter) {
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

    int *col_widths = calloc(max_cols, sizeof(int));
    temp = row;
    for (int i = 0; i < row_count; i++) {
        char *line_copy = strdup(temp->content);
        char *p = line_copy;
        int col = 0;
        char *cell;
        while ((cell = gig_get_next_cell(&p)) && col < max_cols) {
            while (isspace((unsigned char)*cell)) cell++;
            char *end = cell + strlen(cell) - 1;
            while (end > cell && isspace((unsigned char)*end)) *end-- = '\0';
            char *proc = gig_inline_process(cell);
            int vis_len = gig_get_vis_len(proc);
            if (vis_len > col_widths[col]) col_widths[col] = vis_len;
            free(proc);
            col++;
        }
        free(line_copy);
        temp = temp->next;
    }

    int sep_w = (max_cols + 1) + (max_cols - 1); // Vertical lines + spacing
    int avail_w = content_w - 7 - sep_w - (max_cols * 2); // gutter + indent + borders + cell padding
    if (avail_w < max_cols * 4) avail_w = max_cols * 4;

    int total_req = 0;
    for (int i = 0; i < max_cols; i++) total_req += col_widths[i];

    if (total_req > avail_w) {
        int remaining_w = avail_w;
        int remaining_cols = max_cols;
        int *final_widths = calloc(max_cols, sizeof(int));
        char *is_fixed = calloc(max_cols, 1);
        int changed = 1;
        while (changed) {
            changed = 0;
            int share = remaining_w / (remaining_cols ? remaining_cols : 1);
            for (int i = 0; i < max_cols; i++) {
                if (!is_fixed[i] && col_widths[i] <= share) {
                    final_widths[i] = col_widths[i];
                    is_fixed[i] = 1;
                    remaining_w -= col_widths[i];
                    remaining_cols--;
                    changed = 1;
                }
            }
        }
        for (int i = 0; i < max_cols; i++) {
            if (!is_fixed[i]) {
                final_widths[i] = remaining_w / (remaining_cols ? remaining_cols : 1);
                if (final_widths[i] < 4) final_widths[i] = 4;
                remaining_w -= final_widths[i];
                remaining_cols--;
            }
        }
        memcpy(col_widths, final_widths, max_cols * sizeof(int));
        free(final_widths);
        free(is_fixed);
    }

    // Top Border: ┌───┬───┐
    draw_border(layout, col_widths, max_cols, gutter, "\xe2\x94\x8c", "\xe2\x94\xac", "\xe2\x94\x90");

    temp = row;
    for (int r = 0; r < row_count; r++) {
        gig_layout_t **cell_layouts = calloc(max_cols, sizeof(gig_layout_t*));
        int max_h = 0;
        
        char *line_copy = strdup(temp->content);
        char *p_copy = line_copy;
        for (int col = 0; col < max_cols; col++) {
            char *cell = gig_get_next_cell(&p_copy);
            char *trimmed = cell ? cell : "";
            while (isspace((unsigned char)*trimmed)) trimmed++;
            char *end = trimmed + strlen(trimmed) - 1;
            while (end > trimmed && isspace((unsigned char)*end)) *end-- = '\0';

            char *proc = gig_inline_process(trimmed);
            cell_layouts[col] = calloc(1, sizeof(gig_layout_t));
            gig_wrap_text(cell_layouts[col], proc, col_widths[col], 0);
            
            if (cell_layouts[col]->count > max_h) max_h = cell_layouts[col]->count;
            free(proc);
        }
        free(line_copy);

        for (int h = 0; h < max_h; h++) {
            char buf[65536];
            int b_idx = snprintf(buf, sizeof(buf), "%*s%s\xe2\x94\x82%s", gutter + 7, "", GIG_CLR_GREY, GIG_CLR_RESET);
            
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
            draw_border(layout, col_widths, max_cols, gutter, "\xe2\x94\x9c", "\xe2\x94\xbc", "\xe2\x94\xa4");
        } else {
            draw_border(layout, col_widths, max_cols, gutter, "\xe2\x94\x94", "\xe2\x94\xb4", "\xe2\x94\x98");
        }
        temp = temp->next;
    }

    free(col_widths);
    *curr_ptr = temp;
}
