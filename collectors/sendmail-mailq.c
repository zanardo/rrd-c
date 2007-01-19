#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

int main(int argc, char **argv) {
	int nummsg;
	DIR *queue_dir;
	struct dirent *dp;
	char result[256];
	char ip[16];
	int port;

	if(argc < 2) {
		return 1;
	}

	nummsg = 0;

	sscanf(argv[1], "%[^:]:%d", &ip, &port);

	queue_dir = opendir("/var/spool/mqueue/");
	if(queue_dir == NULL)
		return 2;

	while(queue_dir) {
		if ((dp = readdir(queue_dir)) != NULL) {
			if(strlen(dp->d_name) > 1) {
				if(strncmp("qf", dp->d_name, 2) == 0) 
					nummsg++;
			}
		}
		else
			break;
	}

	if(closedir(queue_dir) == -1)
		return 3;

	sprintf(result, "%s N:%d", argv[2], nummsg);
	printf("%s\n", result);

	if(udpsend(ip, port, result) != 0)
		return 3;

	return 0;
}

