#include "wrap.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void gig_wrap_text(gig_layout_t *layout, const char *text, int width, int left_pad) {
    if (!text || !layout) return;

    const char *p = text;
    char line_buf[8192];
    char active_state[256] = ""; // To track ANSI state across lines

    while (*p) {
        int vis_len = 0;
        int buf_idx = 0;
        
        // Add left padding
        for (int i = 0; i < left_pad; i++) line_buf[buf_idx++] = ' ';

        // Re-apply active state at start of line
        if (active_state[0]) {
            int slen = strlen(active_state);
            memcpy(line_buf + buf_idx, active_state, slen);
            buf_idx += slen;
        }

        const char *last_space = NULL;
        int last_space_buf_idx = -1;
        char state_at_space[256] = "";

        while (*p && vis_len < width) {
            if (*p == '\x1b') {
                const char *start = p;
                while (*p && *p != 'm') p++;
                if (*p) p++;
                
                int seq_len = p - start;
                if (buf_idx + seq_len < 8000) {
                    memcpy(line_buf + buf_idx, start, seq_len);
                    buf_idx += seq_len;
                    
                    // Update active state (crude but works for GIG's simple tags)
                    if (strncmp(start, "\x1b[0m", 4) == 0) {
                        active_state[0] = '\0';
                    } else {
                        // Append or replace? For GIG, we usually just have one active block color
                        strncpy(active_state, start, seq_len);
                        active_state[seq_len] = '\0';
                    }
                }
            } else {
                if (*p == ' ') {
                    last_space = p;
                    last_space_buf_idx = buf_idx;
                    strcpy(state_at_space, active_state);
                }
                line_buf[buf_idx++] = *p++;
                vis_len++;
            }
        }

        // Handle word wrap
        if (*p && vis_len >= width && last_space) {
            buf_idx = last_space_buf_idx;
            p = last_space + 1;
            strcpy(active_state, state_at_space);
        }

        line_buf[buf_idx] = '\0';
        
        // Ensure we reset ANSI state at the end of every line
        strcat(line_buf, "\x1b[0m");
        
        void gig_layout_add_line(gig_layout_t *layout, const char *line);
        gig_layout_add_line(layout, line_buf);

        while (*p == ' ') p++; // Skip spaces after wrap
    }
}
