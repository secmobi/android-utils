#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DexParser.h"

int 
main(int argc, char *argv[])
{
	FILE *fp = NULL;
	size_t size = 0;
	unsigned char *buf = NULL;
	void *dex = NULL;

	if(argc < 2)
	{
		fprintf(stderr, "Usage: %s <dex_file> [class|method|string]\n", argv[0]);
		return -1;
	}

	fp = fopen(argv[1], "rb");
	if(fp == NULL)
	{
		fprintf(stderr, "Error: open file %s failed.\n", argv[1]);
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	buf = (unsigned char *)malloc(size);
	if(buf == NULL)
	{
		fprintf(stderr, "Error: alloc memory failed.\n");
		fclose(fp);
		return -1;
	}

	if(fread(buf, 1, size, fp) != size)
	{
		fprintf(stderr, "Error: read file %s failed.\n", argv[1]);
		free(buf);
		fclose(fp);
		return -1;
	}

	dex = DexOpen(buf, size);
	if(dex == NULL)
	{
		free(buf);
		fclose(fp);
		return -1;
	}

	if(argc >= 3)
	{
		if(strcmp(argv[2], "string") == 0)
			DexDumpString(dex);
		else if(strcmp(argv[2], "class") == 0)
			DexDumpClass(dex);
		else if(strcmp(argv[2], "method") == 0)
			DexDumpMethod(dex);
		else
			DexDumpAll(dex);
	}
	else
		DexDumpAll(dex);

	DexClose(dex);

	free(buf);
	fclose(fp);

	return 0;
}
