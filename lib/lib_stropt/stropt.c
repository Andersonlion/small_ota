#include "stropt.h"
#include <string.h>
#include <ctype.h>

/* case-insensitive string compare (replace strncasecmp for bare-metal) */
static int str_n_case_cmp(const char *s1, const char *s2, size_t n)
{
    while (n--) {
        char c1 = tolower((unsigned char)*s1++);
        char c2 = tolower((unsigned char)*s2++);
        if (c1 != c2 || c1 == '\0') {
            return (unsigned char)c1 - (unsigned char)c2;
        }
    }
    return 0;
}

uint32_t stropt_parse_content_length(const char *header, size_t header_len)
{
    const char *pattern = "Content-Length:";
    size_t pattern_len = 15;  /* strlen("Content-Length:") */
    size_t i;

    for (i = 0; i + pattern_len < header_len; i++) {
        /* case-insensitive match */
        if (str_n_case_cmp(&header[i], pattern, pattern_len) == 0) {
            /* skip whitespace after colon */
            i += pattern_len;
            while (i < header_len && (header[i] == ' ' || header[i] == '\t')) {
                i++;
            }
            /* parse decimal number */
            uint32_t value = 0;
            while (i < header_len && isdigit((unsigned char)header[i])) {
                value = value * 10 + (uint32_t)(header[i] - '0');
                i++;
            }
            return value;
        }
    }

    return 0;
}

int stropt_find_body_start(const uint8_t *data, size_t len)
{
    size_t i;

    if (len < 4) {
        return -1;
    }

    for (i = 0; i <= len - 4; i++) {
        if (data[i] == '\r' && data[i + 1] == '\n'
            && data[i + 2] == '\r' && data[i + 3] == '\n') {
            return (int)(i + 4);
        }
    }

    return -1;
}

int stropt_is_valid(const char *str)
{
    if (str == NULL) {
        return 0;
    }
    if (str[0] == '\0' || (unsigned char)str[0] == 0xFF) {
        return 0;
    }
    return 1;
}
