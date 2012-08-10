#ifndef UNICHAR_HPP
#define UNICHAR_HPP

#define UNICHAR_TYPE_ENABLED

#ifdef UNICHAR_TYPE_ENABLED
typedef unsigned int unichar_t;
int uni_isprint(unichar_t c);
int uni_char_width(unichar_t c);
#else
#include <wchar.h>
#include <wctype.h>
typedef wchar_t unichar_t;
#define uni_isprint(c) iswprint(c)
#define uni_char_width(c) wcwidth(c)
#endif

/* Returns 1 if char was returned, 0 if input is missing bytes, -1 if
   illegal sequence. */
int uni_utf8_get_char_n(const void *input, size_t input_len, unichar_t *chr_r);
/* Returns number of UTF-8 bytes required for this character. */
unsigned int uni_utf8_char_bytes(char first_chr);
/* unichar -> NUL-terminated UTF-8 string */
void uni_ucs4_to_utf8_c(unichar_t chr, char *output, size_t size);

#endif
