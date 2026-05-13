#ifndef GIG_TERMINAL_H
#define GIG_TERMINAL_H

#include <termios.h>

typedef struct {
    int rows;
    int cols;
    struct termios orig_termios;
} gig_term_state_t;

void gig_terminal_setup(gig_term_state_t *state);
void gig_terminal_restore(gig_term_state_t *state);
void gig_terminal_get_size(int *rows, int *cols);

// Cursor and Buffer utilities
void gig_terminal_enter_alt_buffer();
void gig_terminal_exit_alt_buffer();
void gig_terminal_hide_cursor();
void gig_terminal_show_cursor();
void gig_terminal_move_cursor(int row, int col);
void gig_terminal_clear_screen();

#endif
