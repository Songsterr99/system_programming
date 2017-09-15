
/*************************************************************************
LAB 1

Edit this file ONLY!

*************************************************************************/

#include "dns.h" 
#include <stdio.h> 
#include <malloc.h> 
#include <string.h> 

#define DNS_SIZE 60000 

typedef struct List {
	char* host_name;
	IPADDRESS host_address;
	struct List *next;
} LItem;

int hash(char *str) // hash function djb2
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; // hash * 33 + c

	return hash % DNS_SIZE;
}

DNSHandle InitDNS()
{
	return (DNSHandle)calloc(DNS_SIZE, sizeof(LItem));
}

void LoadHostsFile(DNSHandle hDNS, const char* hostsFilePath)
{
	FILE* file;
	file = fopen(hostsFilePath, "r");
	if (file == NULL)
		return 1;

	while (!feof(file))
	{
		char hostName[201];
		unsigned int ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0;

		fscanf_s(file, "%d.%d.%d.%d %s", &ip1, &ip2, &ip3, &ip4, hostName, 200);
		IPADDRESS IP_ = (ip1 & 0xFF) << 24 | (ip2 & 0xFF) << 16 | (ip3 & 0xFF) << 8 | (ip4 & 0xFF);
		//IPADDRESS IP_= ip1 * 256 * 256 * 256 + ip2 * 256 * 256 + ip3 * 256 + ip4;

		int uHostNameLength = strlen(hostName);
		const char* host_name = (char*)malloc(uHostNameLength + 1);//-> 
		strcpy(host_name, hostName);

		LItem **e = &((LItem**)hDNS)[hash(hostName)];

		while (*e != NULL) {
			e = &(*e)->next;
		}
		*e = (LItem*)malloc(sizeof(LItem));
		(*e)->host_address = IP_;
		(*e)->host_name = host_name;
		(*e)->next = NULL;
		//push element end 
	}
	fclose(file);
}

void ShutdownDNS(DNSHandle hDNS)
{
	for (int i = 0; i < DNS_SIZE; i++)
	{
		LItem * item = ((LItem**)hDNS)[i];
		while (item != NULL) {
			LItem *help = item;
			item = item->next;
			free(help);
		}
	}
	free((LItem**)hDNS);
}

IPADDRESS DnsLookUp(DNSHandle hDNS, const char* hostName)
{
	LItem *item = ((LItem**)hDNS)[hash(hostName)];
	while (item != NULL)
	{
		if (!strcmp(item->host_name, hostName))
			return item->host_address;
		item = item->next;
	}

	return INVALID_IP_ADDRESS;
}