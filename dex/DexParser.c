#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "DexParser.h"
#include "utils.h"

#define DEX_MAGIC "dex\n035\0"

typedef struct{
	uint8_t 	magic[8];
	uint32_t 	checksum;
	uint8_t		signature[20];
	uint32_t	fileSize;
	uint32_t	headerSize;
	uint32_t	endianTag;
	uint32_t 	linkSize;
	uint32_t	linkOff;
	uint32_t	mapOff;
	uint32_t	stringIdsSize;
	uint32_t	stringIdsOff;
	uint32_t	typeIdsSize;
	uint32_t	typeIdsOff;
	uint32_t	protoIdsSize;
	uint32_t	protoIdsOff;
	uint32_t	fieldIdsSize;
	uint32_t	fieldIdsOff;
	uint32_t	methodIdsSize;
	uint32_t	methodIdsOff;
	uint32_t	classDefsSize;
	uint32_t	classDefsOff;
	uint32_t	dataSize;
	uint32_t	dataOff;
} DexHeader_t;

typedef struct{
	uint32_t	stringDataOff;
} DexStringId_t;

typedef struct{
	uint32_t	descriptorIdx;
} DexTypeId_t;

typedef struct{
	uint32_t 	shortyIdx;
	uint32_t	returnTypeIdx;
	uint32_t	parametersOff;
} DexProtoId_t;

typedef struct{
	uint16_t	typeIdx;
} DexTypeItem_t;

typedef struct{
	uint32_t	size;
	DexTypeItem_t	list[1];
} DexTypeList_t;

typedef struct{
	uint16_t	classIdx;
	uint16_t	typeIdx;
	uint32_t	nameIdx;
} DexFieldId_t;

typedef struct{
	uint16_t	classIdx;
	uint16_t	protoIdx;
	uint32_t	nameIdx;
} DexMethodId_t;

typedef struct{
	uint32_t	classIdx;
	uint32_t	accessFlags;
	uint32_t	superclassIdx;
	uint32_t	interfacesOff;
	uint32_t	sourceFileIdx;
	uint32_t	annotationsOff;
	uint32_t	classDataOff;
	uint32_t	staticValuesOff;
} DexClassDef_t;

typedef struct{
	uint8_t		bleargh;
} DexLink_t;

typedef struct{
	unsigned char *data;
	size_t len;

	DexHeader_t 	*header;
	DexStringId_t 	*stringIds;
	DexTypeId_t	*typeIds;
	DexProtoId_t 	*protoIds;
	DexFieldId_t	*fieldIds;
	DexMethodId_t	*methodIds;
	DexClassDef_t 	*classDefs;
	DexLink_t 	*linkData;
} DexFile_t;

void *
DexOpen(unsigned char *buf, size_t size)
{
	DexFile_t * dex = NULL;

	if(buf == NULL || size == 0)
		return NULL;

	if((dex = (DexFile_t *)malloc(sizeof(DexFile_t))) == NULL)
		return NULL;

	dex->data = buf;
	dex->len = size;

	dex->header = (DexHeader_t *)(dex->data);

	if(dex->len < sizeof(DexHeader_t) || dex->len != dex->header->fileSize)
	{
		free(dex);
		return NULL;
	}

	if(memcmp(dex->header->magic, DEX_MAGIC, 8) != 0)
	{
		free(dex);
		return NULL;
	}

	if(Adler32Checksum(dex->data+12, dex->len-12) != dex->header->checksum)
	{
		free(dex);
		return NULL;
	}

	dex->stringIds = (DexStringId_t *)(dex->data + dex->header->stringIdsOff);
	dex->typeIds = (DexTypeId_t *)(dex->data + dex->header->typeIdsOff);
	dex->protoIds = (DexProtoId_t *)(dex->data + dex->header->protoIdsOff);
	dex->fieldIds = (DexFieldId_t *)(dex->data + dex->header->fieldIdsOff);
	dex->methodIds = (DexMethodId_t *)(dex->data + dex->header->methodIdsOff);
	dex->classDefs = (DexClassDef_t *)(dex->data + dex->header->classDefsOff);
	dex->linkData = (DexLink_t *)(dex->data + dex->header->linkOff);

	return (void *)dex;
}

void 
DexClose(void *dexFile)
{
	DexFile_t *dex = (DexFile_t *)dexFile;

	free(dex);
}

static char *
GetString(DexFile_t *dex, uint32_t idx)
{
	DexStringId_t *id;
	unsigned char *p;

	if(idx >= dex->header->stringIdsSize)
		return NULL;

	id = dex->stringIds + idx;
	if(id->stringDataOff >= dex->len)
		return NULL;

	p = dex->data + id->stringDataOff;

	while (*(p++) > 0x7f)
		;
	return (char *)p;
}

static char *
GetType(DexFile_t *dex, uint32_t idx)
{
	DexTypeId_t *id;

	if(idx >= dex->header->typeIdsSize)
		return NULL;

	id = dex->typeIds + idx;
	return GetString(dex, id->descriptorIdx);
}

static char *
GetProtoShorty(DexFile_t *dex, uint32_t idx)
{
	DexProtoId_t *id;

	if(idx >= dex->header->protoIdsSize)
		return NULL;

	id = dex->protoIds + idx;
	return GetString(dex, id->shortyIdx);
}

static char *
GetProtoReturnType(DexFile_t *dex, uint32_t idx)
{
	DexProtoId_t *id;

	if(idx >= dex->header->protoIdsSize)
		return NULL;

	id = dex->protoIds + idx;
	return GetType(dex, id->returnTypeIdx);
}

static uint32_t 
GetProtoParameterCount(DexFile_t *dex, uint32_t idx)
{
	DexProtoId_t *id;
	DexTypeList_t *para;

	if(idx >= dex->header->protoIdsSize)
		return -1;

	id = dex->protoIds + idx;
	if(id->parametersOff >= dex->len)
		return -1;

	if(id->parametersOff == 0)
		return 0;

	para = (DexTypeList_t *)(dex->data + id->parametersOff);
	return para->size;
}

static char *
GetProtoParameter(DexFile_t *dex, uint32_t idx, uint32_t n)
{
	DexProtoId_t *id;
	DexTypeList_t *para;

	if(idx >= dex->header->protoIdsSize)
		return NULL;

	id = dex->protoIds + idx;
	if(id->parametersOff >= dex->len)
		return NULL;
	if(id->parametersOff == 0)
		return NULL;

	para = (DexTypeList_t *)(dex->data + id->parametersOff);
	if(n >= para->size)
		return NULL;
	
	return GetType(dex, para->list[n].typeIdx);
}

static char *
GetFieldClass(DexFile_t *dex, uint32_t idx)
{
	DexFieldId_t *id;

	if(idx >= dex->header->fieldIdsSize)
		return NULL;

	id = dex->fieldIds + idx;
	return GetType(dex, id->classIdx);
}

static char *
GetFieldType(DexFile_t *dex, uint32_t idx)
{
	DexFieldId_t *id;

	if(idx >= dex->header->fieldIdsSize)
		return NULL;

	id = dex->fieldIds + idx;
	return GetType(dex, id->typeIdx);
}

static char *
GetFieldName(DexFile_t *dex, uint32_t idx)
{
	DexFieldId_t *id;

	if(idx >= dex->header->fieldIdsSize)
		return NULL;

	id = dex->fieldIds + idx;
	return GetString(dex, id->nameIdx);
}

static char *
GetMethodName(DexFile_t *dex, uint32_t idx)
{
	DexMethodId_t *id;

	if(idx >= dex->header->methodIdsSize)
		return NULL;

	id = dex->methodIds + idx;
	return GetString(dex, id->nameIdx);
}

static char *
GetMethodClass(DexFile_t *dex, uint32_t idx)
{
	DexMethodId_t *id;

	if(idx >= dex->header->methodIdsSize)
		return NULL;

	id = dex->methodIds + idx;
	return GetType(dex, id->classIdx);
}

static uint32_t
GetMethodProtoId(DexFile_t *dex, uint32_t idx)
{
	DexMethodId_t *id;

	if(idx >= dex->header->methodIdsSize)
		return -1; 

	id = dex->methodIds + idx;
	return id->protoIdx;
}
static char *
GetClassName(DexFile_t *dex, uint32_t idx)
{
	DexClassDef_t *def;

	if(idx >= dex->header->classDefsSize)
		return "";

	def = dex->classDefs + idx;
	return GetType(dex, def->classIdx);
}

void 
DexDumpAll(void *dexFile)
{
	DexFile_t *dex = (DexFile_t *)dexFile;
	uint32_t i;

	for(i = 0; i < dex->header->stringIdsSize; i++)
		printf("string[%u] = %s\n", i, GetString(dex, i));

	for(i = 0; i < dex->header->typeIdsSize; i++)
		printf("type[%u] = %s\n", i, GetType(dex, i));

	for(i = 0; i < dex->header->protoIdsSize; i++)
	{
		uint32_t j;
		printf("proto[%u] = %s, ret = %s, ", 
				i, 
				GetProtoShorty(dex, i), 
				GetProtoReturnType(dex, i));
		for(j = 0; j < GetProtoParameterCount(dex, i); j++)
			printf("para[%u] = %s, ", j+1, GetProtoParameter(dex, i, j));
		printf("\n");
	}

	for(i = 0; i < dex->header->fieldIdsSize; i++)
		printf("field[%u] = %s -> %s (%s)\n",
				i,
				GetFieldClass(dex, i),
				GetFieldName(dex, i),
				GetFieldType(dex, i));

	for(i = 0; i < dex->header->methodIdsSize; i++)
		printf("method[%u] = %s -> %s (%s)\n",
				i,
				GetMethodClass(dex, i),
				GetMethodName(dex, i),
				GetProtoShorty(dex, GetMethodProtoId(dex, i)));
}

void 
DexDumpString(void *dexFile)
{
	DexFile_t *dex = (DexFile_t *)dexFile;
	uint32_t i;

	for(i = 0; i < dex->header->stringIdsSize; i++)
		printf("%s\n", GetString(dex, i));
}

void 
DexDumpClass(void *dexFile)
{
	DexFile_t *dex = (DexFile_t *)dexFile;
	uint32_t i;

	for(i = 0; i < dex->header->classDefsSize; i++)
		printf("%s\n", GetClassName(dex, i));
}

void 
DexDumpMethod(void *dexFile)
{
	DexFile_t *dex = (DexFile_t *)dexFile;
	uint32_t i;

	for(i = 0; i < dex->header->methodIdsSize; i++)
		printf("%s %s (%s)\n",
				GetMethodClass(dex, i),
				GetMethodName(dex, i),
				GetProtoShorty(dex, GetMethodProtoId(dex, i)));
}
