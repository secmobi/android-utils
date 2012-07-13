#ifndef DEFPARSER_UTILS_H
#define DEFPARSER_UTILS_H

typedef unsigned char uleb128_t[4];

uint32_t Adler32Checksum(unsigned char *data, size_t len);

uint32_t ReadULEB128(unsigned char **data);

#endif
