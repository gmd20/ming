#ifndef XML_ESCAPE_H_
#define XML_ESCAPE_H_

#include <string.h>

namespace ming {
class small_buffer
{
public:
	small_buffer():size_(0),extra_buf_((char*) &data_)
	{

	}
	~small_buffer()
	{
		if(extra_buf_ != (char *) &data_) {
			delete [] extra_buf_;
		}
	}
	void buf_grow(unsigned int size)
	{
		if (size > 1024) {
			extra_buf_ = new char[size];
		}
	}
	void put(const void * b, unsigned int len)
	{
		memcpy(extra_buf_ + size_, b, len);
		size_ += len;
	}
	void putchar(char c)
	{
		extra_buf_[size_++] = c;
	}
	void puts(const char * string)
	{
		put(string, strlen(string));
	}
	char * data()
	{
		return extra_buf_;
	}
	unsigned int size()
	{
		return size_;
	}
private:
	char data_[1024];
	unsigned int size_;
	char *extra_buf_;
};

bool
escape_xml(small_buffer &ob, const unsigned char *src, unsigned int size);
bool
unescape_xml(small_buffer &ob, const char *src, unsigned int size);

} // naamespace ming
#endif // XML_ESCAPE_H_
