/*	Public domain	*/

#ifndef _AGAR_UNICODE_H_
#define _AGAR_UNICODE_H_
#include "begin_code.h"

enum ag_unicode_conv {
	AG_UNICODE_FROM_USASCII,	/* US-ASCII -> UCS-4 */
	AG_UNICODE_FROM_UTF8,		/* UTF-8 -> UCS-4 */
	AG_UNICODE_TO_UTF8		/* UCS-4 -> UTF-8 */
};

__BEGIN_DECLS
Uint32	*AG_ImportUnicode(enum ag_unicode_conv, const char *, size_t);
long     AG_ExportUnicode(enum ag_unicode_conv, char *, const Uint32 *, size_t)
	     BOUNDED_ATTRIBUTE(__string__, 2, 4);
size_t	 AG_CopyUnicode(enum ag_unicode_conv, const char *, Uint32 *, size_t);

size_t	 AG_UCS4Copy(Uint32 *, const Uint32 *, size_t);
size_t	 AG_UCS4Cat(Uint32 *, const Uint32 *, size_t);
Uint32	*AG_UCS4Dup(const Uint32 *);
Uint32	*AG_UCS4Sep(Uint32 **, const Uint32 *);

/*
 * Return the length of a UCS-4 string in characters, without the
 * terminating NUL.
 */
static __inline__ size_t
AG_LengthUCS4(const Uint32 *ucs)
{
	size_t len;

	for (len = 0; *ucs != '\0'; ucs++) {
		len++;
	}
	return (len);
}

/*
 * Return the number of bytes that would be needed to encode the given
 * UCS-4 character in UTF-8.
 */
static __inline__ int
AG_CharLengthUTF8FromUCS4(Uint32 ch)
{
	if      (ch <  0x80)		{ return (1); }
	else if (ch <  0x800)		{ return (2); }
	else if (ch <  0x10000)		{ return (3); }
	else if (ch <  0x200000)	{ return (4); }
	else if (ch <  0x4000000)	{ return (5); }
	else if (ch <= 0x7fffffff)	{ return (6); }
	else				{ AG_FatalError("CharLengthUTF8"); }
}

/*
 * Return the number of bytes (not including the terminating NUL) that would
 * be needed to encode the given UCS-4 string in UTF-8.
 */
static __inline__ size_t
AG_LengthUTF8FromUCS4(const Uint32 *ucs4)
{
	size_t rv = 0;
	const Uint32 *c;

	for (c = &ucs4[0]; *c != '\0'; c++) {
		rv += AG_CharLengthUTF8FromUCS4(*c);
	}
	return (rv);
}

/*
 * Parse the first byte of a possible UTF-8 sequence and return the length
 * of the sequence in bytes (or 1 if there is none).
 */
static __inline__ int
AG_CharLengthUTF8(Uint8 ch)
{
	if ((ch >> 7) == 0) {
		return (1);
	} else if (((ch & 0xe0) >> 5) == 0x6) {
		return (2);
	} else if (((ch & 0xf0) >> 4) == 0xe) {
		return (3);
	} else if (((ch & 0xf8) >> 3) == 0x1e) {
		return (4);
	} else if (((ch & 0xfc) >> 2) == 0x3e) {
		return (5);
	} else if (((ch & 0xfe) >> 1) == 0x7e) {
		return (6);
	} else {
		return (-1);
	}
}

/*
 * Return the number of characters in the given UTF-8 string, not counting
 * the terminating NUL.
 */
static __inline__ size_t
AG_LengthUTF8(const char *s)
{
	const char *c = &s[0];
	size_t rv = 0;
	int i, cLen;

	if (s[0] == '\0') {
		return (0);
	}
	for (;;) {
		if ((cLen = AG_CharLengthUTF8(*c)) == -1) {
			break;
		}
		for (i = 0; i < cLen; i++) {
			if (c[i] == '\0')
				return (rv);
		}
		rv++;
		c += cLen;
	}
	return (rv);
}
__END_DECLS

#include "close_code.h"
#endif /* _AGAR_UNICODE_H_ */
