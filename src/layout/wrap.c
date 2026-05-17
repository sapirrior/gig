#include "wrap.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/**
 * Returns the number of bytes in the current UTF-8 character.
 */
static int get_utf8_char_len(unsigned char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xe0) == 0xc0) return 2;
    if ((c & 0xf0) == 0xe0) return 3;
    if ((c & 0xf8) == 0xf0) return 4;
    return 1;
}

/**
 * High priority: Space
 * Medium priority: Punctuation (break after)
 * Low priority: Path/URL separators (break after)
 */
static int get_break_after_priority(char c) {
    if (c == ' ') return 3;
    if (c == ',' || c == ';' || c == ':' || c == '!' || c == '?') return 2;
    if (c == '/' || c == '\\' || c == '.' || c == '-' || c == '_' || c == '@') return 1;
    return 0;
}

/**
 * Break before opening brackets/parens.
 */
static int is_break_before(char c) {
    return (c == '(' || c == '[' || c == '{' || c == '<');
}

void gig_wrap_text(gig_layout_t *layout, const char *text, int width, int left_pad) {
    if (!text || !layout) return;
    if (width < 1) width = 1;

    const char *p = text;
    char line_buf[32768]; 
    char active_state[1024] = ""; 

    while (*p) {
        int vis_len = 0;
        int buf_idx = 0;
        const char *p_at_line_start = p;
        
        // 1. Initial padding
        for (int i = 0; i < left_pad; i++) line_buf[buf_idx++] = ' ';

        // 2. State restoration
        if (active_state[0]) {
            int slen = strlen(active_state);
            memcpy(line_buf + buf_idx, active_state, slen);
            buf_idx += slen;
        }

        const char *best_break_p = NULL;
        int best_break_buf_idx = -1;
        char state_at_break[1024] = "";
        int current_max_priority = 0;

        while (*p && vis_len < width) {
            if (*p == '\x1b') {
                const char *seq_start = p;
                while (*p && *p != 'm') p++;
                if (*p) p++;
                
                int len = p - seq_start;
                if (buf_idx + len < 32000) {
                    memcpy(line_buf + buf_idx, seq_start, len);
                    buf_idx += len;
                    
                    if (strncmp(seq_start, "\x1b[0m", 4) == 0) {
                        active_state[0] = '\0';
                    } else if (strlen(active_state) + len < 1022) {
                        strncat(active_state, seq_start, len);
                    }
                }
                continue;
            }

            // Check for "Break Before" characters
            if (is_break_before(*p) && vis_len > 0) {
                if (current_max_priority <= 1) {
                    best_break_p = p;
                    best_break_buf_idx = buf_idx;
                    strcpy(state_at_break, active_state);
                    current_max_priority = 1; 
                }
            }

            // Consume character (UTF-8 safe)
            int char_len = get_utf8_char_len((unsigned char)*p);
            for (int i = 0; i < char_len; i++) {
                if (*p) line_buf[buf_idx++] = *p++;
            }
            vis_len++;

            // Check for "Break After" characters
            int priority = get_break_after_priority(*(p-1));
            if (priority > 0 && priority >= current_max_priority) {
                best_break_p = p;
                best_break_buf_idx = buf_idx;
                strcpy(state_at_break, active_state);
                current_max_priority = priority;
            }
        }

        // 3. Wrap Decision
        if (*p && vis_len >= width) {
            if (best_break_p && (best_break_p > p_at_line_start)) {
                // We have a valid breakpoint
                buf_idx = best_break_buf_idx;
                p = best_break_p;
                strcpy(active_state, state_at_break);
                
                // If we broke at a space, skip it for the next line
                if (current_max_priority == 3) {
                    while (*p == ' ') p++;
                }
            } else {
                // No breakpoint found, we already advanced p in the inner loop.
                // p is currently at the character that pushed vis_len >= width.
                // We just stop here and continue on next line.
                // This is the "emergency force break".
                strcpy(active_state, active_state); // State remains active
            }
        }

        line_buf[buf_idx] = '\0';
        
        // 4. Reset style for terminal safety
        strcat(line_buf, "\x1b[0m");
        
        void gig_layout_add_line(gig_layout_t *layout, const char *line);
        gig_layout_add_line(layout, line_buf);
    }
}
