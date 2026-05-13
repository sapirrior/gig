#include "input.h"
#include "ui/colors.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <poll.h>

int gig_pager_peek_key() {
    struct pollfd pfd = { STDIN_FILENO, POLLIN, 0 };
    int ret = poll(&pfd, 1, 0);
    if (ret > 0) return gig_pager_read_key();
    return -2; // Same as EINTR/No key available
}

int gig_pager_read_key() {
    char c;
    ssize_t n = read(STDIN_FILENO, &c, 1);
    if (n <= 0) return (n == 0) ? -1 : (errno == EINTR ? -2 : -1);

    if (c == '\x1b') {
        char seq[8];
        if (read(STDIN_FILENO, &seq[0], 1) <= 0) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) <= 0) return '\x1b';

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) <= 0) return '\x1b';
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '1': return 'g'; 
                        case '4': return 'G'; 
                        case '5': return 'b'; 
                        case '6': return 'f'; 
                    }
                }
            } else {
                switch (seq[1]) {
                    case 'A': return 'k'; 
                    case 'B': return 'j'; 
                    case 'C': return 'l'; 
                    case 'D': return 'h'; 
                    case 'H': return 'g'; 
                    case 'F': return 'G'; 
                }
            }
        } else if (seq[0] == 'O') {
            switch (seq[1]) {
                case 'H': return 'g'; 
                case 'F': return 'G'; 
            }
        }
        return '\x1b';
    }
    return c;
}

void gig_pager_prompt(gig_term_state_t *ts, const char *prompt, char *buf, int size) {
    int idx = 0;
    buf[0] = '\0';

    while (1) {
        gig_terminal_move_cursor(ts->rows, 1);
        printf("%s%s%s\x1b[K", GIG_CLR_STATUS, prompt, buf);
        fflush(stdout);

        char c;
        if (read(STDIN_FILENO, &c, 1) <= 0) break;

        if (c == '\n' || c == '\r') break;
        if (c == 27) { 
            buf[0] = '\0';
            break;
        }
        if (c == 127 || c == 8) { 
            if (idx > 0) buf[--idx] = '\0';
        } else if (idx < size - 1 && c >= 32 && c <= 126) {
            buf[idx++] = c;
            buf[idx] = '\0';
        }
    }
}
