#include <stdio.h>

int main(int argc, char **argv) {
	FILE *fp_loadavg;
	char line[1024];
	float m1, m5, m15;
	char result[256];
	char ip[16];
	int port;

	if(argc < 2) {
		return 1;
	}

	sscanf(argv[1], "%[^:]:%d", &ip, &port);

	fp_loadavg = fopen("/proc/loadavg", "r");
	if(fp_loadavg == NULL) 
		return 2;
	
	fgets(line, sizeof(line)-1, fp_loadavg);
	sscanf(line, "%f %f %f ", &m1, &m5, &m15);

	fclose(fp_loadavg);

	sprintf(result, "%s N:%.2f:%.2f:%.2f", argv[2], m1, m5, m15);
	printf("%s\n", result);

	if(udpsend(ip, port, result) != 0)
		return 3;

	return 0;
}
