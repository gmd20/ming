// protocol-buffer varint and zigzag https://developers.google.com/protocol-buffers/docs/encoding
// https://github.com/powturbo/TurboPFor
// https://github.com/ot/partitioned_elias_fano
// https://github.com/lemire/FastPFor
// Gorilla: A Fast, Scalable, In-Memory Time Series Database http://www.vldb.org/pvldb/vol8/p1816-teller.pdf
// keyword:  varint, zigzag, elias fano, Run-length encoding (RLE), FastPFor, Index Compression Using 64-Bit Words: Simple-8b 



void to_varint(uint64 n, unsigned char* buf)
{
    while (n > 127)
    {
        *buf++ = (n & 0x7F) | 0x80;
        n >>= 7;
    }
    *buf = n;
}

uint64 from_varint(unsigned char* buf)
{
    uint64 n = 0;
    int shift = 0;
    while (*buf & 0x80)
    {
        n |= static_cast<uint64>(*buf++ & 0x7F) << shift;
        shift += 7;
    }
    n |= *buf << shift;
    return n;
}


void to_zigzag(int64 n, unsigned char* buf)
{
    uint64 z = (n << 1) ^ (n >> (BIT_WIDTH - 1));
    while (z > 127)
    {
        *buf++ = (z & 0x7F) | 0x80;
        z >>= 7;
    }
    *buf = z;
}

int64 from_zigzag(unsigned char* buf)
{
    uint64 z = 0;
    int shift = 0;
    while (*buf & x80)
    {
        z |= static_cast<uint64>(*buf++ & 0x7F) << shift;
        shift += 7;
    }
    z |= *buf << shift;
    return (z & 1) ? (z >> 1) ^ -1 : (z >> 1);
}
