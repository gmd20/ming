#include "xml_escape.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

namespace ming {
/**
 * & --> &amp;
 * < --> &lt;
 * > --> &gt;
 * " --> &quot;
 * ' --> &apos;
 */
static const char *LOOKUP_CODES[] = {
	"", /* reserved: use literal single character */
	"", /* unused */
	"", /* reserved: 2 character UTF-8 */
	"", /* reserved: 3 character UTF-8 */
	"", /* reserved: 4 character UTF-8 */
	"?", /* invalid UTF-8 character */
	"&quot;",
	"&amp;",
	"&apos;",
	"&lt;",
	"&gt;"
};

static const unsigned char CODE_INVALID = 5;

static const unsigned char XML_LOOKUP_TABLE[] = {
	/* ASCII: 0xxxxxxx */
	5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 5, 5, 0, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	0, 0, 6, 0, 0, 0, 7, 8, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0,10, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	/* Invalid UTF-8 char start: 10xxxxxx */
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,

	/* Multibyte UTF-8 */

	/* 2 bytes: 110xxxxx */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

	/* 3 bytes: 1110xxxx */
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

	/* 4 bytes: 11110xxx */
	4, 4, 4, 4, 4, 4, 4, 4,

	/* Invalid UTF-8: 11111xxx */
	5, 5, 5, 5, 5, 5, 5, 5,
};

// ---------------------------------------
// if escape string was found in the sr buffer, ob will be set  and  return true
// else ob will not set and return false
//----------------------------------------
bool
escape_xml(small_buffer &ob, const unsigned char *src, unsigned int size)
{
	unsigned int i = 0;
	unsigned char code = 0;

	// most of the time no replacement is needed,  optimize for the fast path
	while (i < size) {
		unsigned int byte;
		byte = src[i++];
		code = XML_LOOKUP_TABLE[byte];
		if (code) {
			goto REAL_WORK;
		}
	}
	return false;

REAL_WORK:
	i--;
	ob.buf_grow(size * 8);
	if (i > 0) {
		ob.put(src,i);
	}

	while (i < size) {
		unsigned int start, end;
		start = end = i;

		while (i < size) {
			unsigned int byte;

			byte = src[i++];
			code = XML_LOOKUP_TABLE[byte];

			if (!code) {
				/* single character used literally */
			} else if (code >= CODE_INVALID) {
				break; /* insert lookup code string */
			} else if (code > size - end) {
				code = CODE_INVALID; /* truncated UTF-8 character */
				break;
			} else {
				unsigned int chr = byte & (0xff >> code);

				while (--code) {
					byte = src[i++];
					if ((byte & 0xc0) != 0x80) {
						code = CODE_INVALID;
						break;
					}
					chr = (chr << 6) + (byte & 0x3f);
				}

				switch (i - end) {
					case 2:
						if (chr < 0x80)
							code = CODE_INVALID;
						break;
					case 3:
						if (chr < 0x800 ||
							(chr > 0xd7ff && chr < 0xe000) ||
							chr > 0xfffd)
							code = CODE_INVALID;
						break;
					case 4:
						if (chr < 0x10000 || chr > 0x10ffff)
							code = CODE_INVALID;
						break;
					default:
						break;
				}
				if (code == CODE_INVALID)
					break;
			}

			end = i;
		}

		if (end > start)
			ob.put(src + start, end - start);

		/* escaping */
		if (end >= size)
			break;

		ob.puts(LOOKUP_CODES[code]);
	}

	return true;
}

/**
 *  &amp;  -->  &
 *  &lt;   -->  <
 *  &gt;   -->  >
 *  &quot; -->  "
 *  &apos; -->  '
 */
// ---------------------------------------
// if escape string was found in the sr buffer, ob will be set  and  return true
// else ob will not set and return false
//----------------------------------------
bool
unescape_xml(small_buffer &ob, const char *src, unsigned int size)
{
	unsigned int i = 0;
	unsigned int start = 0;
	while (i < size) {
		if (src[i] != '&') {
			i++;
			continue;
		} else {
			goto REAL_WORK;
		}
	}
	return false;

REAL_WORK:
	ob.buf_grow(size);

MATCH_LOOP:

	char code = 0;
	unsigned int len = 0;
	do {
		if (i + 3 < size) {
			unsigned int key =  *(unsigned int *) &src[i];
			if (key ==  ('&' | 'l' << 8 | 't' << 16 | ';' << 24)) {
				code = '<';
				len = 4;
				break;
			}
			if (key == ('&' | 'g' << 8 | 't' << 16 | ';' << 24)) {
				code = '>';
				len = 4;
				break;
			}
		} else {
			break;
		}

		if (i + 4 < size) {
			unsigned int key =  *(unsigned int *) &src[i+1];
			if (key ==  ('a' | 'm' << 8 | 'p' << 16 | ';' << 24) ) {
				code = '&';
				len = 5;
				break;
			}
		} else {
			break;
		}

		if (i + 5 < size && src[i+5] == ';') {
			unsigned int key =  *(unsigned int *) &src[i+1];
			if (key == ('q' | 'u' << 8 | 'o' << 16 | 't' << 24)) {
				code = '"';
				len = 6;
				break;
			}
			if (key == ('a' | 'p' << 8 | 'o' << 16 | 's' << 24)) {
				code = '\'';
				len = 6;
				break;
			}
		} else {
			break;
		}
	} while(0);

	if (code) {
		if (i > start) {
			ob.put(&src[start], i - start);
		}
		ob.putc(code);
		i += len;
		start = i;
	} else {
		i++;
	}

	while (i < size) {
		if (src[i] != '&') {
			i++;
			continue;
		} else {
			goto MATCH_LOOP;
		}
	}

	if (i > start) {
		ob.put(&src[start], i - start);
	}

	return true;
}

} // namespace ming
