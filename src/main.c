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

#include "help/help.h"

int main(int argc, char *argv[]) {
    // Set internal GIGPATH for transparency
    setenv("GIGPATH", gig_get_path_string(), 1);

    if (argc < 2) {
        gig_help_print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "path") == 0) {
        printf("%s%s%s\n", GIG_CLR_ARG, gig_get_path_string(), GIG_CLR_RESET);
        return 0;
    }

    if (strcmp(argv[1], "sections") == 0) {
        printf("\n%sGIG Section Reference:%s\n", GIG_CLR_HEADER, GIG_CLR_RESET);
        printf("  %s1%s  User Commands\n", GIG_CLR_CMD, GIG_CLR_RESET);
        printf("  %s2%s  System Calls\n", GIG_CLR_CMD, GIG_CLR_RESET);
        printf("  %s3%s  Library Functions\n", GIG_CLR_CMD, GIG_CLR_RESET);
        printf("  %s4%s  Special Files & Devices\n", GIG_CLR_CMD, GIG_CLR_RESET);
        printf("  %s5%s  File Formats & Conventions\n", GIG_CLR_CMD, GIG_CLR_RESET);
        printf("  %s6%s  Games & Demos\n", GIG_CLR_CMD, GIG_CLR_RESET);
        printf("  %s7%s  Miscellaneous & Specifications\n", GIG_CLR_CMD, GIG_CLR_RESET);
        printf("  %s8%s  System Administration\n", GIG_CLR_CMD, GIG_CLR_RESET);
        printf("  %s9%s  Kernel Routines\n", GIG_CLR_CMD, GIG_CLR_RESET);
        printf("  %sn%s  New / Pending Guides\n", GIG_CLR_CMD, GIG_CLR_RESET);
        printf("  %sl%s  Local / Project Specific\n\n", GIG_CLR_CMD, GIG_CLR_RESET);
        return 0;
    }

    if (strcmp(argv[1], "version") == 0 || strcmp(argv[1], "--version") == 0) {
        printf("%sgig %s%s\n", GIG_CLR_BOLD, GIG_VERSION, GIG_CLR_RESET);
        return 0;
    }

    char *target_path = NULL;

    if (strcmp(argv[1], "help") == 0) {
        gig_help_print_full();
        return 0;
    } else if (strcmp(argv[1], "view") == 0) {
        if (argc < 3) {
            fprintf(stderr, "%sfatal:%s missing file operand for 'view'\n", GIG_CLR_ERROR, GIG_CLR_RESET);
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
                fprintf(stderr, "%sfatal:%s '%s' is not a valid section\n", GIG_CLR_ERROR, GIG_CLR_RESET, argv[1]);
                fprintf(stderr, "%shint:%s run 'gig sections' to see valid sections\n", GIG_CLR_WARN, GIG_CLR_RESET);
                return 1;
            }
        }

        target_path = gig_locate_page(section, name);
    }

    if (!target_path) {
        fprintf(stderr, "%sfatal:%s no guide found for '%s'\n", GIG_CLR_ERROR, GIG_CLR_RESET, argv[argc-1]);
        return 1;
    }

    // 1. Parse
    gig_doc_t *doc = gig_parse_file(target_path);
    if (!doc) {
        fprintf(stderr, "%sfatal:%s could not access '%s'\n", GIG_CLR_ERROR, GIG_CLR_RESET, target_path);
        free(target_path);
        return 1;
    }

    if (doc->error.message) {
        fprintf(stderr, "%svalidation failed:%s %s (line %d)\n", GIG_CLR_ERROR, GIG_CLR_RESET, doc->error.message, doc->error.line_num);
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
