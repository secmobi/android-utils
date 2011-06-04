/* AXML Printer
 * https://github.com/claudxiao/AndTools
 * Claud Xiao <iClaudXiao@gmail.com>
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "AxmlParser.h"
#include "AxmlPrinter.h"

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
