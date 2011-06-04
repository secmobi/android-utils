#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "AxmlPrinter.h"

int main(int argc, char *argv[])
{
	FILE *fp;
	char *buffer;
	size_t size;
	size_t ret;

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
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	buffer = (char *)malloc(size * sizeof(char));
	if(buffer == NULL)
	{
		fprintf(stderr, "Error: init file buffer.\n");
		fclose(fp);
		return -1;
	}

	ret = fread(buffer, 1, size, fp);
	if(ret != size)
	{
		fprintf(stderr, "Error: read file.\n");
		free(buffer);
		fclose(fp);
		return -1;
	}

	ret = AxmlPrinter(buffer, size);

	free(buffer);
	fclose(fp);
	
	return ret;
}
