/* AXML Printer
 * https://github.com/claudxiao/AndTools
 * Claud Xiao <iClaudXiao@gmail.com>
 */
#ifndef AXMLPRINTER_H
#define AXMLPRINTER_H

#include "AxmlParser.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

int AxmlPrinter(char *buffer, size_t size);

int AxmlPrinter2(char *inbuf, size_t insize, char **outbuf, size_t *outsize);

#ifdef __cplusplus
#if __cplusplus
};
#endif
#endif

#endif
