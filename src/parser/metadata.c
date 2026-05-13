#include "metadata.h"
#include <string.h>
#include <ctype.h>

int gig_parse_meta_line(const char *line, gig_doc_t *doc) {
    if (!line || line[0] != '@' || !doc) return -1; // Internal error

    // 1. Check for space after key
    const char *first_space = strchr(line, ' ');
    if (!first_space) return -2; // Format: missing space after @key

    // 2. Extract key
    char key[128];
    const char *k_start = line + 1;
    size_t k_len = first_space - k_start;
    if (k_len == 0) return -3; // Format: empty key
    if (k_len >= sizeof(key)) return -4; // Format: key too long
    
    memcpy(key, k_start, k_len);
    key[k_len] = '\0';

    // 3. Extract value (must be in quotes)
    const char *v_open = strchr(first_space, '"');
    if (!v_open) return -5; // Format: missing opening quote
    
    const char *v_close = strrchr(v_open + 1, '"');
    if (!v_close || v_close == v_open) return -6; // Format: missing closing quote

    size_t v_len = v_close - (v_open + 1);
    if (v_len >= 1024) return -7; // Format: value exceeds 1024 bytes

    char val[1024];
    memcpy(val, v_open + 1, v_len);
    val[v_len] = '\0';

    // 4. Strict Metadata Whitelist
    if (strcmp(key, "name") != 0 && 
        strcmp(key, "section") != 0 && 
        strcmp(key, "version") != 0 && 
        strcmp(key, "author") != 0 && 
        strcmp(key, "updated") != 0) {
        return -8; // Validation: unauthorized key
    }

    // 5. Store
    doc->metadata = realloc(doc->metadata, sizeof(gig_meta_t) * (doc->meta_count + 1));
    doc->metadata[doc->meta_count].key = strdup(key);
    doc->metadata[doc->meta_count].value = strdup(val);
    doc->meta_count++;
    return 0; // Success
}

const char* gig_get_meta(gig_doc_t *doc, const char *key) {
    if (!doc || !key) return NULL;
    for (int i = 0; i < doc->meta_count; i++) {
        if (strcmp(doc->metadata[i].key, key) == 0) return doc->metadata[i].value;
    }
    return NULL;
}
