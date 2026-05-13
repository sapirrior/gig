#ifndef GIG_HELP_H
#define GIG_HELP_H

/**
 * Prints the short usage summary to stdout.
 * @param prog The name of the program (argv[0]).
 */
void gig_help_print_usage(const char *prog);

/**
 * Prints the full interactive help guide to stdout with colors.
 */
void gig_help_print_full(void);

#endif
