#ifndef DEXPARSER_H
#define DEXPARSER_H

void *DexOpen(unsigned char *buf, size_t size);
void DexClose(void *dexFile);

void DexDumpAll(void *dexFile);
void DexDumpString(void *dexFile);
void DexDumpClass(void *dexFile);
void DexDumpMethod(void *dexFile);

#endif
