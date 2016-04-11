#ifndef MING_ENCODING_H_
#define MING_ENCODING_H_
namespace ming {
namespace encoding {

int hex_encode(char *dest, const char *str, int len);
int hex_decode(char *dest, const char *str, int len);

int latin1_to_utf8(char *dest, char const *src);
int latin1_to_utf8(char *dest, int *dest_len, const char *src, int *src_len);
int utf8_to_latin1(char *dest, int *dest_len, const char *src, int *src_len);

// standard UCS2/UTF16
int utf8_to_ucs2(char *outb, int *outlen, const char *in, int *inlen);
int ucs2_to_utf8(char *outb, int *outlen, const char *in, int *inlen);

// UCS2-little endian, Windows and linux's default UNICODE ?
int utf8_to_ucs2le(char *outb, int *outlen, const char *in, int *inlen);
int ucs2le_to_utf8(char *outb, int *outlen, const char *in, int *inlen);

// GSM 03.38 7-bit default alphabet encoding.
// The octets have not been packed together to remove the highest unused bit
// yet.
// This is okay in SMPP ptotocol since the SMPP gateway can do that for us,
// but in other cases extra 7-bit compacting may be needed.
int gsm_to_utf8(char *dest, int *dest_len, const char *src, int *src_len);
int utf8_to_gsm(char *dest, int *dest_len, const char *src, int *src_len);

}  // namespace encoding
}  // namespace ming

#endif  // MING_ENCODING_H_
