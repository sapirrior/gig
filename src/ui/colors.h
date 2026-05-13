#ifndef GIG_COLORS_H
#define GIG_COLORS_H

// GIG Palette: Organic Warmth (Inspired by Claude.com)
// Uses soft creams, sage greens, and warm terracotta for a friendly, professional feel.

#define GIG_CLR_HEADER   "\x1b[38;5;230;1m"  // Bold Warm Cream
#define GIG_CLR_SUB      "\x1b[38;5;223;1m"  // Bold Sand / Light Tan
#define GIG_CLR_STATUS   "\x1b[48;5;95;38;5;230;1m" // Deep Clay BG, Cream FG
#define GIG_CLR_CMD      "\x1b[38;5;108m"    // Soft Sage Green (Actions)
#define GIG_CLR_FLAG     "\x1b[38;5;108m"    // Same Sage for consistency
#define GIG_CLR_ARG      "\x1b[38;5;173m"    // Soft Terracotta / Clay (Values)
#define GIG_CLR_STRING   "\x1b[38;5;173m"    // Unified Terracotta
#define GIG_CLR_GREY     "\x1b[38;5;246m"    // Soft Warm Grey (Punctuation)
#define GIG_CLR_LINK     "\x1b[38;5;109;4m"  // Muted Slate Blue (Underlined)
#define GIG_CLR_QUOTE    "\x1b[38;5;143;3m"  // Muted Olive Green Italic
#define GIG_CLR_LITERAL  "\x1b[38;5;108m"    // Soft Sage Green (Code/Literal)
#define GIG_CLR_BOLD     "\x1b[1m"           // Bold
#define GIG_CLR_ERROR    "\x1b[38;5;167;1m"  // Bold Terracotta Red
#define GIG_CLR_WARN     "\x1b[38;5;214m"    // Warm Orange/Yellow
#define GIG_CLR_SEARCH   "\x1b[48;5;208;38;5;232m" // Bright Orange BG, Black FG
#define GIG_CLR_RESET    "\x1b[0m"

#endif
