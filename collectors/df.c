#include <stdio.h>
#include <sys/statvfs.h>

int main(int argc, char **argv) {
	struct statvfs stat;
	long used, free, pused;
	char result[256];
	char ip[16];
	int port;

	if(argc < 3) {
		return 1;
	}

	sscanf(argv[1], "%[^:]:%d", &ip, &port);

	if(statvfs(argv[3], &stat) != 0)
		return 1;

	used = stat.f_blocks;
	free = stat.f_bfree;
	pused = 100 - (long)((float)free / (float)used * 100);

	sprintf(result, "%s N:%d", argv[2], pused);
	printf("%s\n", result);

	if(udpsend(ip, port, result) != 0)
		return 2;

	return 0;
}
