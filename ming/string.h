#ifndef MING_STRING_H_

staic inline size_t strlcat(char *dest, const char *src, size_t buf_len)
{
	size_t len1 = strlen(dest);
	size_t len2 = strlen(src);
	size_t len = len1 + len2;

	dest += len1;
	buf_len -= len1;
	if (len2 >= buf_len) {
		len2 = buf_len-1;
	}
	memcpy(dest, src, len2);
	dest[len2] = 0;
	return len;
}


#endif 
