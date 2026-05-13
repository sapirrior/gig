#define _GNU_SOURCE
#include "pager.h"
#include "terminal.h"
#include "render.h"
#include "input.h"
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

static volatile sig_atomic_t resize_pending = 0;

static void handle_sigwinch(int sig) {
    (void)sig;
    resize_pending = 1;
}

void gig_pager_run(gig_doc_t *doc, gig_layout_t *layout) {
    gig_term_state_t ts;
    gig_terminal_setup(&ts);
    gig_terminal_get_size(&ts.rows, &ts.cols);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigwinch;
    sigaction(SIGWINCH, &sa, NULL);

    int scroll_y = 0;
    int running = 1;
    int needs_redraw = 1;
    char search_query[256] = "";

    while (running) {
        if (resize_pending) {
            gig_terminal_get_size(&ts.rows, &ts.cols);
            gig_layout_t *new_layout = gig_layout_build(doc, ts.cols);
            if (new_layout) {
                gig_layout_free(layout);
                layout = new_layout;
            }
            resize_pending = 0;
            needs_redraw = 1;
        }

        if (needs_redraw) {
            gig_pager_draw_screen(doc, layout, &ts, scroll_y, search_query);
            needs_redraw = 0;
        }

        int key = gig_pager_read_key();
        if (key == -1) break;    
        if (key == -2) continue; 

        int viewport_h = ts.rows - 4; 
        int max_scroll = layout->count - viewport_h;
        if (max_scroll < 0) max_scroll = 0;

        switch (key) {
            case '/':
                gig_pager_prompt(&ts, "/", search_query, sizeof(search_query));
                needs_redraw = 1;
            case 'n':
                if (search_query[0]) {
                    for (int i = scroll_y + 1; i < layout->count; i++) {
                        if (strcasestr(layout->lines[i], search_query)) {
                            scroll_y = i;
                            needs_redraw = 1;
                            break;
                        }
                    }
                }
                break;
            
            case 'N':
                if (search_query[0]) {
                    for (int i = scroll_y - 1; i >= 0; i--) {
                        if (strcasestr(layout->lines[i], search_query)) {
                            scroll_y = i;
                            needs_redraw = 1;
                            break;
                        }
                    }
                }
                break;

            case 'q':
            case 'Q':
                running = 0;
                break;

            case 'j':
            case 'e':
            case 5:   // CTRL-E
            case 14:  // CTRL-N
            case '\r':
            case '\n':
                if (scroll_y < max_scroll) { scroll_y++; needs_redraw = 1; }
                break;

            case 'k':
            case 'y':
            case 25:  // CTRL-Y
            case 16:  // CTRL-P
                if (scroll_y > 0) { scroll_y--; needs_redraw = 1; }
                break;

            case 'd':
            case 4:   // CTRL-D
                scroll_y += viewport_h / 2;
                if (scroll_y > max_scroll) scroll_y = max_scroll;
                needs_redraw = 1;
                break;

            case 'u':
            case 21:  // CTRL-U
                scroll_y -= viewport_h / 2;
                if (scroll_y < 0) scroll_y = 0;
                needs_redraw = 1;
                break;

            case 'f':
            case ' ':
            case 6:   // CTRL-F
            case 22:  // CTRL-V
                scroll_y += viewport_h;
                if (scroll_y > max_scroll) scroll_y = max_scroll;
                needs_redraw = 1;
                break;

            case 'b':
            case 2:   // CTRL-B
                scroll_y -= viewport_h;
                if (scroll_y < 0) scroll_y = 0;
                needs_redraw = 1;
                break;

            case 'g':
            case '<':
                scroll_y = 0;
                needs_redraw = 1;
                break;

            case 'G':
            case '>':
                scroll_y = max_scroll;
                needs_redraw = 1;
                break;
            
            case 'r':
            case 12: // CTRL-L
            case 18: // CTRL-R
                needs_redraw = 1;
                break;

            case 'Z': 
                if (gig_pager_read_key() == 'Z') running = 0;
                break;
            
            case ':': 
                if (gig_pager_read_key() == 'q') running = 0;
                break;

            case '\x1b': 
                {
                    int next = gig_pager_read_key();
                    if (next == 'v') { 
                        scroll_y -= viewport_h;
                        if (scroll_y < 0) scroll_y = 0;
                        needs_redraw = 1;
                    }
                }
                break;
        }
        
        if (scroll_y > max_scroll) scroll_y = max_scroll;
    }

    gig_terminal_restore(&ts);
}
