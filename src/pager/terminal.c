#include "terminal.h"
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>

void gig_terminal_setup(gig_term_state_t *state) {
    tcgetattr(STDIN_FILENO, &state->orig_termios);
    struct termios raw = state->orig_termios;
    
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    gig_terminal_enter_alt_buffer();
    gig_terminal_hide_cursor();
}

void gig_terminal_restore(gig_term_state_t *state) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &state->orig_termios);
    gig_terminal_exit_alt_buffer();
    gig_terminal_show_cursor();
}

void gig_terminal_get_size(int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        *rows = 24;
        *cols = 80;
    } else {
        *rows = ws.ws_row;
        *cols = ws.ws_col;
    }
}

void gig_terminal_enter_alt_buffer() { write(STDOUT_FILENO, "\x1b[?1049h", 8); }
void gig_terminal_exit_alt_buffer()  { write(STDOUT_FILENO, "\x1b[?1049l", 8); }
void gig_terminal_hide_cursor()      { write(STDOUT_FILENO, "\x1b[?25l", 6); }
void gig_terminal_show_cursor()      { write(STDOUT_FILENO, "\x1b[?25h", 6); }
void gig_terminal_move_cursor(int row, int col) {
    char buf[32];
    int n = sprintf(buf, "\x1b[%d;%dH", row, col);
    write(STDOUT_FILENO, buf, n);
}
void gig_terminal_clear_screen()     { write(STDOUT_FILENO, "\x1b[2J", 4); }
