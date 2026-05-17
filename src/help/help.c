#include "help.h"
#include "ui/colors.h"
#include <stdio.h>

void gig_help_print_usage(const char *prog) {
    printf("%susage:%s %s [%ssection%s] <%sname%s>\n", GIG_CLR_WARN, GIG_CLR_RESET, prog, GIG_CLR_ARG, GIG_CLR_RESET, GIG_CLR_ARG, GIG_CLR_RESET);
    printf("       %s %sview%s <%sfile.gg%s>\n", prog, GIG_CLR_CMD, GIG_CLR_RESET, GIG_CLR_ARG, GIG_CLR_RESET);
    printf("       %s %ssync%s\n", prog, GIG_CLR_CMD, GIG_CLR_RESET);
    printf("       %s %spath%s\n", prog, GIG_CLR_CMD, GIG_CLR_RESET);
    printf("       %s %sversion%s\n", prog, GIG_CLR_CMD, GIG_CLR_RESET);
    printf("       %s %ssections%s\n", prog, GIG_CLR_CMD, GIG_CLR_RESET);
    printf("       %s %shelp%s\n", prog, GIG_CLR_CMD, GIG_CLR_RESET);
}


void gig_help_print_full(void) {
    printf("\n%sWELCOME TO GIG!%s\n", GIG_CLR_HEADER, GIG_CLR_RESET);
    printf("    Welcome to your new favorite way to explore documentation! %sGIG%s is\n", GIG_CLR_BOLD, GIG_CLR_RESET);
    printf("    designed to be your faithful companion as you navigate through technical\n");
    printf("    guides and manuals. Let's get you ready for your journey!\n\n");

    printf("%sNAVIGATING THE PATH%s\n", GIG_CLR_HEADER, GIG_CLR_RESET);
    printf("    Moving around is easy and feels just like your favorite pager:\n");
    printf("    %s•%s %sj%s / %sk%s     : %sStep Down%s / %sUp%s\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET);
    printf("    %s•%s %sd%s / %su%s     : %sLeap Down%s / %sUp%s (Half-page)\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET);
    printf("    %s•%s %sf%s / %sb%s     : %sFly Down%s / %sUp%s (Full-page)\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET);
    printf("    %s•%s %sg%s / %sG%s     : %sStart%s / %sEnd%s of document\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET);
    printf("    %s•%s %s/%s         : Search for treasures (text)\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET);
    printf("    %s•%s %sn%s / %sN%s     : %sNext%s / %sPrev%s match\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET);
    printf("    %s•%s %sq%s         : Exit your journey\n\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET);

    printf("%sTHE LANGUAGE OF GIG%s\n", GIG_CLR_HEADER, GIG_CLR_RESET);
    printf("    Documentation uses simple \"triggers\" to make everything beautiful:\n");
    printf("    %s#%s  : %sThe Great Header%s (Primary landmark)\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET);
    printf("    %s##%s : %sThe Sub Header%s (Local signpost)\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET);
    printf("    %s!%s  : %sThe Literal Block%s (Raw technical data, no wrapping)\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET);
    printf("    %s.%s  : %sA simple story%s (Standard paragraph)\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET);
    printf("    %s*%s  : %sA quick point%s (Bullet item)\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET);
    printf("    %s-%s  : %sA definition%s (- <%sname%s> : desc)\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET, GIG_CLR_ARG, GIG_CLR_RESET);
    printf("    %s$%s  : %sA command%s for your shell\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET);
    printf("    %s>%s  : %sWisdom%s (A block quote)\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET);
    printf("    %s|%s  : %sThe Great Grid%s (Organized tables)\n\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET);

    printf("%sYOUR TOOLKIT%s\n", GIG_CLR_HEADER, GIG_CLR_RESET);
    printf("    %s•%s %sview%s <%sfile%s>  : Open any .gg file on your disk\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET, GIG_CLR_ARG, GIG_CLR_RESET);
    printf("    %s•%s %ssync%s          : Update guides from the cloud\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET);
    printf("    %s•%s %spath%s          : See the guide search path\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET);
    printf("    %s•%s %sversion%s       : Check your %sGIG%s version\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET, GIG_CLR_BOLD, GIG_CLR_RESET);
    printf("    %s•%s %ssections%s      : List manual sections (1-9, n, l)\n\n", GIG_CLR_GREY, GIG_CLR_RESET, GIG_CLR_CMD, GIG_CLR_RESET);

    printf("%sENJOY THE RIDE!%s\n", GIG_CLR_HEADER, GIG_CLR_RESET);
    printf("    GIG uses a %s\"Golden Staircase\"%s layout for readability.\n", GIG_CLR_BOLD, GIG_CLR_RESET);
    printf("    Go forth and explore the manuals!\n\n");
}
