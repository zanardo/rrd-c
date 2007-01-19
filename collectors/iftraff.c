#include <stdio.h>

int main(int argc, char **argv) {
	char result[256];
	char ip[16];
	int port;
	FILE *fp_dev;
	char pattern[256];
	unsigned int v[16];
	char line[1024];

	if(argc < 3) {
		return 1;
	}

	sscanf(argv[1], "%[^:]:%d", &ip, &port);

	fp_dev = fopen("/proc/net/dev", "r");
	if(fp_dev == NULL) 
		return 2;

	strcpy(pattern, " ");
	strcat(pattern, argv[3]);
	strcat(pattern, ": %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u ");

	while( fgets(line, sizeof(line)-1, fp_dev) ) {
		if(sscanf(line, pattern, &v[0],&v[1],&v[2],&v[3],&v[4],&v[5],&v[6],&v[7],
			&v[8],&v[9],&v[10],&v[11],&v[12],&v[13],&v[14],&v[15])) {
			fclose(fp_dev);
			sprintf(result, "%s N:%u:%u", argv[2], v[0], v[8]);
			printf("%s\n", result);
			if(udpsend(ip, port, result) != 0)
				return 3;
			return 0;
		}
	}

	fclose(fp_dev);

	return 4;
}
