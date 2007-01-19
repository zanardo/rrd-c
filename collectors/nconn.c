#include <stdio.h>

int main(int argc, char **argv) {
	FILE *fp_nconn;
	char line[1024];
	int nconn;
	char result[1024];
	char ip[16];
	int port;

	if(argc < 2) {
		return 1;
	}

	sscanf(argv[1], "%[^:]:%d", &ip, &port);

	nconn = 0;

	fp_nconn = fopen("/proc/net/ip_conntrack", "r");
	if(fp_nconn == NULL) 
		return 2;

	while( fgets(line, sizeof(line)-1, fp_nconn) )
		if(strncmp(line, argv[3], strlen(argv[3])) == 0)
			nconn++;
	
	fclose(fp_nconn);

	sprintf(result, "%s N:%d", argv[2], nconn);
	printf("%s\n", result);

	if(udpsend(ip, port, result) != 0)
		return 3;

	return 0;
}
