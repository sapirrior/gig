#include "layout.h"
#include <stdlib.h>
#include <string.h>

void gig_layout_free(gig_layout_t *layout) {
    if (!layout) return;
    for (int i = 0; i < layout->count; i++) {
        free(layout->lines[i]);
    }
    free(layout->lines);
    free(layout);
}

// Internal: Add a rendered line to the layout
void gig_layout_add_line(gig_layout_t *layout, const char *line) {
    if (!layout || !line) return;
    if (layout->count >= layout->capacity) {
        layout->capacity = layout->capacity == 0 ? 32 : layout->capacity * 2;
        layout->lines = realloc(layout->lines, sizeof(char*) * layout->capacity);
    }
    layout->lines[layout->count++] = strdup(line);
}
