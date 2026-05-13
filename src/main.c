#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser/gig.h"
#include "layout/layout.h"
#include "pager/pager.h"
#include "pager/terminal.h"
#include "cli/locator.h"

#include "ui/colors.h"

void print_usage(const char *prog) {
    printf("usage: %s [section] <name>\n", prog);
    printf("       %s view <file.gg>\n", prog);
    printf("       %s path\n", prog);
    printf("       %s version\n", prog);
    printf("       %s sections\n", prog);
    printf("       %s help\n", prog);
}

int main(int argc, char *argv[]) {
    // Set internal GIGPATH for transparency
    setenv("GIGPATH", gig_get_path_string(), 1);

    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "path") == 0) {
        printf("%s\n", gig_get_path_string());
        return 0;
    }

    if (strcmp(argv[1], "sections") == 0) {
        printf("GIG Section Reference:\n");
        printf("  1  User Commands\n");
        printf("  2  System Calls\n");
        printf("  3  Library Functions\n");
        printf("  4  Special Files & Devices\n");
        printf("  5  File Formats & Conventions\n");
        printf("  6  Games & Demos\n");
        printf("  7  Miscellaneous & Specifications\n");
        printf("  8  System Administration\n");
        printf("  9  Kernel Routines\n");
        printf("  n  New / Pending Guides\n");
        printf("  l  Local / Project Specific\n");
        return 0;
    }

    if (strcmp(argv[1], "version") == 0 || strcmp(argv[1], "--version") == 0) {
        printf("gig %s\n", GIG_VERSION);
        return 0;
    }

    char *target_path = NULL;

    if (strcmp(argv[1], "help") == 0) {
        target_path = strdup("src/help/help.gg");
    } else if (strcmp(argv[1], "view") == 0) {
        if (argc < 3) {
            fprintf(stderr, "fatal: missing file operand for 'view'\n");
            return 1;
        }
        target_path = strdup(argv[2]);
    } else {
        const char *section = NULL;
        const char *name = NULL;

        if (argc == 2) {
            // gig <name>
            name = argv[1];
        } else {
            // gig <section> <name>
            if (gig_is_valid_section(argv[1])) {
                section = argv[1];
                name = argv[2];
            } else {
                fprintf(stderr, "fatal: '%s' is not a valid section\n", argv[1]);
                fprintf(stderr, "hint: run 'gig sections' to see valid sections\n");
                return 1;
            }
        }

        target_path = gig_locate_page(section, name);
    }

    if (!target_path) {
        fprintf(stderr, "fatal: no guide found for '%s'\n", argv[argc-1]);
        return 1;
    }

    // 1. Parse
    gig_doc_t *doc = gig_parse_file(target_path);
    if (!doc) {
        fprintf(stderr, "fatal: could not access '%s'\n", target_path);
        free(target_path);
        return 1;
    }

    if (doc->error.message) {
        fprintf(stderr, "validation failed: %s (line %d)\n", doc->error.message, doc->error.line_num);
        gig_free_doc(doc);
        free(target_path);
        return 1;
    }

    // 2. Layout
    int rows, cols;
    gig_terminal_get_size(&rows, &cols);
    gig_layout_t *layout = gig_layout_build(doc, cols);
    
    if (layout) {
        // 3. Page
        gig_pager_run(doc, &layout);
        gig_layout_free(layout);
    }

    gig_free_doc(doc);
    free(target_path);

    return 0;
}
