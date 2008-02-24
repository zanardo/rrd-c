#include <stdio.h>
#include <stdlib.h>

int obtem_sinal(char *dev) {
	FILE *fp_dev;
	char pattern[256];
	unsigned int snr, tmp;
	char line[1024];

	fp_dev = fopen("/proc/net/wireless", "r");
	if(fp_dev == NULL) 
		return 2;

	strcpy(pattern, " ");
	strcat(pattern, dev);
	strcat(pattern, ": %u %u.");

	while( fgets(line, sizeof(line)-1, fp_dev) ) {
		if(sscanf(line, pattern, &tmp, &snr)) {
			fclose(fp_dev);
			return(snr);
		}
	}
	fclose(fp_dev);
	return(-1);
}

int main(int argc, char **argv) {
	char result[256];
	char ip[16];
	int i;
	int port;
	int snr;
	int val[60];
	int med, min, max, soma;

	min = 92;
	max = 0;

	if(argc < 3) {
		return 1;
	}

	sscanf(argv[1], "%[^:]:%d", &ip, &port);

	for(i=0; i<60; i++) {
		snr = obtem_sinal(argv[3]);
		if(snr == -1)
			return 2;
		val[i] = snr;	
		sleep(1);
	}

	soma = 0;
	for(i=0; i<60; i++) {
		soma += val[i];
		if(val[i] > max) max = val[i];
		if(val[i] < min) min = val[i];
	}

	med = soma/60;

	sprintf(result, "%s N:%d:%d:%d", argv[2], med, min, max);
	printf("%s\n", result);
	if(udpsend(ip, port, result) != 0)
		return 3;
	return 0;
}
