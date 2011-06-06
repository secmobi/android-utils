/* AXML Printer
 * https://github.com/claudxiao/AndTools
 * Claud Xiao <iClaudXiao@gmail.com>
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "AxmlParser.h"
#include "AxmlPrinter.h"

#ifdef _WIN32		/* windows */
#pragma warning(disable:4996)
#endif

int 
AxmlPrinter(char *buffer, size_t size)
{
	void *axml;
	AxmlEvent_t event;

	int tabCnt = 0;

	axml = AxmlOpen(buffer, size);
	if(axml == NULL)
		return -1;

	while((event = AxmlNext(axml)) != AE_ENDDOC)
	{
		char *prefix;
		char *name;
		char *value;
		uint32_t i, n;

		switch(event){
		case AE_STARTDOC:
			printf("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
			break;
			
		case AE_STARTTAG:
			printf("%*s", (tabCnt++)*4, "");

			prefix = AxmlGetTagPrefix(axml);
			name = AxmlGetTagName(axml);
			if(strlen(prefix) != 0)
				printf("<%s:%s ", prefix, name);
			else
				printf("<%s ", name);

			if(AxmlNewNamespace(axml))
				printf("xmlns:%s=\"%s\" ", AxmlGetNsPrefix(axml), AxmlGetNsUri(axml));

			n = AxmlGetAttrCount(axml);
			for(i = 0; i < n; i++)
			{
				prefix = AxmlGetAttrPrefix(axml, i);
				name = AxmlGetAttrName(axml, i);
				value = AxmlGetAttrValue(axml, i);

				if(strlen(prefix) != 0)
					printf("%s:%s=\"%s\" ", prefix, name, value);
				else
					printf("%s=\"%s\" ", name, value);

				/* must manually free attribute value here */
				free(value);
			}

			printf(">\n");
			break;

		case AE_ENDTAG:
			printf("%*s", (--tabCnt)*4, "");

			prefix = AxmlGetTagPrefix(axml);
			name = AxmlGetTagName(axml);
			if(strlen(prefix) != 0)
				printf("</%s:%s>\n", prefix, name);
			else
				printf("</%s>\n", name);
			break;

		case AE_TEXT:
			printf("%s\n", AxmlGetText(axml));
			break;

		case AE_ERROR:
			fprintf(stderr, "Error: AxmlNext() returns a AE_ERROR event.\n");
			AxmlClose(axml);
			return -1;
			break;

		default:
			break;
		}
	}

	AxmlClose(axml);

	return 0;
}

int 
AxmlPrinter2(char *inbuf, size_t insize, char **outbuf, size_t *outsize)
{
	FILE *fp;
	char *tmpfile = "temp.dat";

	int ret;

	fp = freopen(tmpfile, "w", stdout);
	if(fp == NULL)
	{
		fprintf(stderr, "Error: cannot redirect stdout to temp file.\n");
		return -1;
	}

	ret = AxmlPrinter(inbuf, insize);

#ifndef _WIN32		/* linux */
	fp = freopen("/dev/tty", "w", stdout);
	if(fp == NULL)
	{
		fprintf(stderr, "Fatal Error: cannot redirect stdout to console.\n");
		remove(tmpfile);
		return -2;
	}
#else			/* windows */
	fp = freopen("CON", "w", stdout);
	/* Do not check if freopen sucessfully in this situation. 
	 * Refer to Microsoft KB58667 */
#endif

	if(ret != 0)
	{
		remove(tmpfile);
		return ret;
	}

	fp = fopen(tmpfile, "rb");
	if(fp == NULL)
	{
		fprintf(stderr, "Error: cannot open temp file.\n");
		remove(tmpfile);
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	*outsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	*outbuf = (char *)malloc(*outsize + 1);
	if(*outbuf == NULL)
	{
		fprintf(stderr, "Error: cannot init outbuf.\n");
		fclose(fp);
		remove(tmpfile);
		*outbuf = NULL;
		*outsize = 0;
		return -1;
	}

	ret = fread(*outbuf, 1, *outsize, fp);
	if(ret != *outsize)
	{
		fprintf(stderr, "Error: cannot read complete temp file.\n");
		fclose(fp);
		remove(tmpfile);
		free(*outbuf);
		*outbuf = NULL;
		*outsize = 0;
		return -1;
	}

	(*outbuf)[*outsize] = '\0';
	(*outsize)++;

	fclose(fp);
	remove(tmpfile);
	return 0;
}
