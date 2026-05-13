#include "inline.h"
#include "ui/colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* --- Helper: Check if we are at a word boundary --- */
static int is_boundary(const char *start, const char *curr) {
    if (curr == start) return 1;
    unsigned char prev = (unsigned char)*(curr - 1);
    return isspace(prev) || prev == '(' || prev == '[' || prev == '{' || prev == '|' || prev == '=';
}

/* --- Helper: Check for Flag Pattern --- */
static int is_flag(const char *start, const char *curr) {
    if (*curr != '-') return 0;
    if (!is_boundary(start, curr)) return 0;
    
    // --flag
    if (curr[1] == '-' && isalnum((unsigned char)curr[2])) return 1;
    // -f
    if (isalnum((unsigned char)curr[1])) return 1;
    
    return 0;
}

/* --- Helper: Find closing char with safety --- */
static const char* find_closer(const char *s, char closer, int max_dist) {
    const char *p = s;
    int dist = 0;
    while (*p && *p != '\n' && dist < max_dist) {
        if (*p == closer) return p;
        p++;
        dist++;
    }
    return NULL;
}

char* gig_inline_process(const char *text) {
    if (!text) return strdup("");

    // Estimate buffer size: ANSI codes add significant overhead
    char *res = malloc(strlen(text) * 16 + 2048);
    char *dst = res;
    const char *src = text;
    const char *start = text;

    while (*src) {
        // 0. Handle Escapes
        if (*src == '\\' && src[1] != '\0') {
            *dst++ = src[1];
            src += 2;
            continue;
        }

        // 0.5. Bold: **text**
        if (*src == '*' && src[1] == '*') {
            const char *end = strstr(src + 2, "**");
            if (end) {
                dst += sprintf(dst, "%s", GIG_CLR_BOLD);
                src += 2;
                while (src < end) *dst++ = *src++;
                dst += sprintf(dst, "%s", GIG_CLR_RESET);
                src += 2;
                continue;
            }
        }

        // 1. Commands: `command`
        if (*src == '`') {
            const char *end = find_closer(src + 1, '`', 512);
            if (end) {
                dst += sprintf(dst, "%s", GIG_CLR_CMD);
                src++;
                while (src < end) *dst++ = *src++;
                dst += sprintf(dst, "%s", GIG_CLR_RESET);
                src++;
                continue;
            }
        }

        // 2. Arguments: <variable> - Stricter: no spaces allowed usually
        if (*src == '<') {
            const char *end = find_closer(src + 1, '>', 64);
            if (end) {
                // Check if it's likely a variable (no spaces or very few)
                int has_space = 0;
                for (const char *t = src + 1; t < end; t++) if (isspace((unsigned char)*t)) has_space = 1;
                
                if (!has_space) {
                    dst += sprintf(dst, "%s", GIG_CLR_ARG);
                    *dst++ = *src++;
                    while (src < end) *dst++ = *src++;
                    *dst++ = *src++;
                    dst += sprintf(dst, "%s", GIG_CLR_RESET);
                    continue;
                }
            }
        }

        // 3. Optional: [optional]
        if (*src == '[') {
            const char *end = find_closer(src + 1, ']', 128);
            if (end) {
                dst += sprintf(dst, "%s", GIG_CLR_GREY);
                *dst++ = *src++;
                while (src < end) {
                    // Nested coloring for flags/args inside brackets
                    if (is_flag(start, src)) {
                        dst += sprintf(dst, "%s", GIG_CLR_FLAG);
                        while (src < end && !isspace((unsigned char)*src) && *src != ',' && *src != '|' && *src != '=') *dst++ = *src++;
                        dst += sprintf(dst, "%s%s", GIG_CLR_RESET, GIG_CLR_GREY);
                    } else if (*src == '<') {
                        const char *v_end = find_closer(src + 1, '>', 32);
                        if (v_end && v_end < end) {
                            dst += sprintf(dst, "%s", GIG_CLR_ARG);
                            while (src <= v_end) *dst++ = *src++;
                            dst += sprintf(dst, "%s%s", GIG_CLR_RESET, GIG_CLR_GREY);
                        } else *dst++ = *src++;
                    } else {
                        *dst++ = *src++;
                    }
                }
                if (*src == ']') *dst++ = *src++;
                dst += sprintf(dst, "%s", GIG_CLR_RESET);
                continue;
            }
        }

        // 4. Links: {target(section)}
        if (*src == '{') {
            const char *end = find_closer(src + 1, '}', 128);
            if (end) {
                dst += sprintf(dst, "%s", GIG_CLR_LINK);
                src++;
                while (src < end) *dst++ = *src++;
                dst += sprintf(dst, "%s", GIG_CLR_RESET);
                src++;
                continue;
            }
        }

        // 5. Constants: "literal"
        if (*src == '"') {
            const char *end = find_closer(src + 1, '"', 256);
            if (end) {
                dst += sprintf(dst, "%s", GIG_CLR_STRING);
                *dst++ = *src++;
                while (src < end) *dst++ = *src++;
                *dst++ = *src++;
                dst += sprintf(dst, "%s", GIG_CLR_RESET);
                continue;
            }
        }

        // 6. Auto-sensing Flags: -f or --flag
        if (is_flag(start, src)) {
            dst += sprintf(dst, "%s", GIG_CLR_FLAG);
            while (*src && !isspace((unsigned char)*src) && *src != ',' && *src != ']' && *src != ')' && *src != '|' && *src != '=' && *src != ':') {
                *dst++ = *src++;
            }
            dst += sprintf(dst, "%s", GIG_CLR_RESET);
            continue;
        }

        // 7. General Punctuation Coloring
        if (*src == '(' || *src == ')' || *src == '|' || *src == ',' || *src == '=' || *src == ':') {
            dst += sprintf(dst, "%s%c%s", GIG_CLR_GREY, *src, GIG_CLR_RESET);
            src++;
            continue;
        }

        // Default: Plain character
        *dst++ = *src++;
    }

    *dst = '\0';
    return res;
}
