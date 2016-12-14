#pragma once
#pragma warning(disable:4201)
#pragma warning(disable:4100)
#define NDIS_WDM
#define NDIS630
#include<wdm.h>
#include<ndis.h>


#define NETCFGGUID L"{5cbf81bd-5055-47cd-9055-a76b2b4e3697}"
#define SERVICENAME L"ZlzNdisLwf"
typedef struct _FILTER_CONTEXT
{
	char magic[8];
	NDIS_HANDLE FilterHandle;
	NDIS_HANDLE NetBufferPool;
	BOOLEAN IsRunning;
	int FliterIndex;
}FILTER_CONTEXT, *PFILTER_CONTEXT;
typedef struct _GLOBAL
{
	NDIS_HANDLE DrverHandle;
	int contextnum;
	PFILTER_CONTEXT context[20];
}GLOBAL, *PGLOBAL;
GLOBAL Global;