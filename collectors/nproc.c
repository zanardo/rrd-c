#include <stdio.h>

int main(int argc, char **argv) {
	FILE *fp_nproc;
	char line[1024];
	int running, sleeping;
	float tmp;
	char result[256];
	char ip[16];
	int port;

	if(argc < 2) {
		return 1;
	}

	sscanf(argv[1], "%[^:]:%d", &ip, &port);
	
	fp_nproc = fopen("/proc/loadavg", "r");
	if(fp_nproc == NULL) 
		return 2;

	fgets(line, sizeof(line)-1, fp_nproc);
	sscanf(line, "%f %f %f %d/%d ", &tmp, &tmp, &tmp, &running, &sleeping);

	fclose(fp_nproc);

	sprintf(result, "%s N:%d:%d ", argv[2], running, sleeping);
	printf("%s\n", result);

	if(udpsend(ip, port, result) != 0)
		return 3;

	return 0;
}
