#include <stdio.h>

int main(int argc, char **argv) {
	FILE *fp_meminfo;
	char line[1024];
	int value, i;
	unsigned int memtotal, memfree, cached,
		buffers, swaptotal, swapfree, memused, swapused;
	char result[1024];
	char ip[16];
	int port;

	if(argc < 2)
		return 1;

	sscanf(argv[1], "%[^:]:%d", &ip, &port);
	
	memtotal = memfree = cached = buffers = 
	swaptotal = swapfree = memused = swapused = 0;
	
	fp_meminfo = fopen("/proc/meminfo", "r");
	if(fp_meminfo == NULL) 
		return 2;

	while( fgets(line, sizeof(line)-1, fp_meminfo) ) {
		sscanf(line, "MemTotal: %d kB ", &memtotal);
		sscanf(line, "MemFree: %d kB ", &memfree);
		sscanf(line, "Cached: %d kB ", &cached);
		sscanf(line, "Buffers: %d kB ", &buffers);
		sscanf(line, "SwapTotal: %d kB ", &swaptotal);
		sscanf(line, "SwapFree: %d kB ", &swapfree);
	}
	fclose(fp_meminfo);
	
	memused = memtotal - memfree - cached - buffers;
	swapused = swaptotal - swapfree;

	sprintf(result, "%s N:%d:%d", argv[2], memused, swapused);
	printf("%s\n", result);

	if(udpsend(ip, port, result) != 0)
		return 3;

	return 0;
}
