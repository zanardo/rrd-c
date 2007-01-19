#include <stdio.h>

int main(int argc, char **argv) {
	FILE *fp_cpuinfo;
	char line[1024];
	char result[1024];
	char ip[16];
	int port;
	float mhz;

	if(argc < 2) {
		return 1;
	}

	sscanf(argv[1], "%[^:]:%d", &ip, &port);
	
	fp_cpuinfo = fopen("/proc/cpuinfo", "r");
	if(fp_cpuinfo == NULL) 
		return 2;

	while( fgets(line, sizeof(line)-1, fp_cpuinfo) )
		if(sscanf(line, "cpu MHz : %f ", &mhz))
			break;

	fclose(fp_cpuinfo);
	
	sprintf(result, "%s N:%.0f", argv[2], mhz);
	printf("%s\n", result);

	if(udpsend(ip, port, result) != 0)
		return 3;

	return 0;
}
