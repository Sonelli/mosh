#include <string.h>
#include <assert.h>

#include "unichar.h"

static const unsigned char uni_utf8_non1_bytes[256 - 192 - 2] = {
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

int uni_utf8_get_char_n(const void *_input, size_t input_len, unichar_t *chr_r)
{
	const unsigned char *input = (const unsigned char *)_input;
	unichar_t chr;
	unsigned int i, char_len;
	int ret;

	if (input_len == 0)
		return 0;
	if (*input < 0x80) {
		*chr_r = *input;
		return 1;
	}

	/* first byte has char_len highest bits set, followed by zero bit.
	   the rest of the bits are used as the highest bits of the value. */
	chr = *input;
	char_len = uni_utf8_char_bytes(*input);
	switch (char_len) {
	case 2:
		chr &= 0x1f;
		break;
	case 3:
		chr &= 0x0f;
		break;
	case 4:
		chr &= 0x07;
		break;
	case 5:
		chr &= 0x03;
		break;
	case 6:
		chr &= 0x01;
		break;
	default:
		/* only 7bit chars should have char_len==1 */
		return -1;
	}

	if (char_len <= input_len)
		ret = 1;
	else {
		/* check first if the input is invalid before returning 0 */
		ret = 0;
		char_len = input_len;
	}

	/* the following bytes must all be 10xxxxxx */
	for (i = 1; i < char_len; i++) {
		if ((input[i] & 0xc0) != 0x80)
			return input[i] == '\0' ? 0 : -1;

		chr <<= 6;
		chr |= input[i] & 0x3f;
	}

	*chr_r = chr;
	return ret;
}

unsigned int uni_utf8_char_bytes(char first_chr)
{
	/* 0x00 .. 0x7f are ASCII. 0x80 .. 0xC1 are invalid. */
	if ((unsigned char)first_chr < (192 + 2))
		return 1;
	return uni_utf8_non1_bytes[(unsigned char)first_chr - (192 + 2)];
}

void uni_ucs4_to_utf8_c(unichar_t chr, char *output, size_t size)
{
	unsigned char first;
	int bitpos;

	assert(size >= 6);

	if (chr < 0x80) {
		output[0] = chr;
		output[1] = '\0';
		return;
	}

	if (chr < (1 << (6 + 5))) {
		/* 110xxxxx */
		bitpos = 6;
		first = 0x80 | 0x40;
	} else if (chr < (1 << ((2*6) + 4))) {
		/* 1110xxxx */
		bitpos = 2*6;
		first = 0x80 | 0x40 | 0x20;
	} else if (chr < (1 << ((3*6) + 3))) {
		/* 11110xxx */
		bitpos = 3*6;
		first = 0x80 | 0x40 | 0x20 | 0x10;
	} else if (chr < (1 << ((4*6) + 2))) {
		/* 111110xx */
		bitpos = 4*6;
		first = 0x80 | 0x40 | 0x20 | 0x10 | 0x08;
	} else {
		/* 1111110x */
		bitpos = 5*6;
		first = 0x80 | 0x40 | 0x20 | 0x10 | 0x08 | 0x04;
	}
	*output++ = first | (chr >> bitpos);

	do {
		bitpos -= 6;
		*output++ = 0x80 | ((chr >> bitpos) & 0x3f);
	} while (bitpos > 0);
	*output = '\0';
}

#ifdef UNICHAR_TYPE_ENABLED
int uni_isprint(unichar_t c)
{
  if (c < ' ')
    return 0;
  if (c == 0xc0 || c == 0xc1)
    return 0;
  return 1;
}
#endif
