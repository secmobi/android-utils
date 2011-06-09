#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "AxmlParser.h"

#ifdef _WIN32		/* windows */
#pragma warning(disable:4996)
#endif

int main(int argc, char *argv[])
{
	FILE *fp;
	char *inbuf;
	size_t insize;
	char *outbuf;
	size_t outsize;
	int ret;

	if(argc != 2)
	{
		fprintf(stderr, "Usage: AxmlPrinter <axmlfile>\n");
		return -1;
	}

	fp = fopen(argv[1], "rb");
	if(fp == NULL)
	{
		fprintf(stderr, "Error: open input file failed.\n");
		return -1;
	}
	
	fseek(fp, 0, SEEK_END);
	insize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	inbuf = (char *)malloc(insize * sizeof(char));
	if(inbuf == NULL)
	{
		fprintf(stderr, "Error: init file buffer.\n");
		fclose(fp);
		return -1;
	}

	ret = fread(inbuf, 1, insize, fp);
	if(ret != insize)
	{
		fprintf(stderr, "Error: read file.\n");
		free(inbuf);
		fclose(fp);
		return -1;
	}

	ret = AxmlToXml(&outbuf, &outsize, inbuf, insize);

	free(inbuf);
	fclose(fp);

	if(ret == 0)
		printf("%s", outbuf);
	free(outbuf);
	
	return ret;
}
