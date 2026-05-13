#include "metadata.h"
#include <string.h>
#include <ctype.h>

int gig_parse_meta_line(const char *line, gig_doc_t *doc) {
    if (!line || line[0] != '@' || !doc) return 0;

    // Format check: must have at least one space and quotes
    if (!strchr(line, ' ') || !strchr(line, '"')) return 0;

    // Format: @key    "value"
    char key[128];
    char val[1024];

    // Find end of key (first space)
    const char *k_start = line + 1;
    const char *k_end = k_start;
    while (*k_end && !isspace((unsigned char)*k_end)) k_end++;
    
    size_t k_len = k_end - k_start;
    if (k_len == 0 || k_len >= sizeof(key)) return 0;
    strncpy(key, k_start, k_len);
    key[k_len] = '\0';

    // Find start of value (first quote)
    const char *v_start = strchr(k_end, '"');
    if (!v_start) return 0;
    v_start++; // Skip opening quote

    // Find end of value (last quote)
    const char *v_end = strrchr(v_start, '"');
    if (!v_end || v_end <= v_start) return 0;

    size_t v_len = v_end - v_start;
    if (v_len >= sizeof(val)) v_len = sizeof(val) - 1;
    strncpy(val, v_start, v_len);
    val[v_len] = '\0';

    // --- Strict Metadata Whitelist ---
    if (strcmp(key, "name") != 0 && 
        strcmp(key, "section") != 0 && 
        strcmp(key, "version") != 0 && 
        strcmp(key, "author") != 0 && 
        strcmp(key, "updated") != 0) {
        return -1; // Specific code for unknown key
    }

    // Store in doc
    doc->metadata = realloc(doc->metadata, sizeof(gig_meta_t) * (doc->meta_count + 1));
    doc->metadata[doc->meta_count].key = strdup(key);
    doc->metadata[doc->meta_count].value = strdup(val);
    doc->meta_count++;
    return 1;
}

const char* gig_get_meta(gig_doc_t *doc, const char *key) {
    if (!doc || !key) return NULL;
    for (int i = 0; i < doc->meta_count; i++) {
        if (strcmp(doc->metadata[i].key, key) == 0) return doc->metadata[i].value;
    }
    return NULL;
}
