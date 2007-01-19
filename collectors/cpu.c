#include <stdio.h>
#include <unistd.h>

void getvalues(
	unsigned int *user,
	unsigned int *nice,
	unsigned int *system,
	unsigned int *idle)
{
	FILE *fp_stat;
	char line[1024];

	fp_stat = fopen("/proc/stat", "r");
	if(fp_stat == NULL)
		exit(1);
 	fgets(line, sizeof(line)-1, fp_stat);
	sscanf(line, "cpu %u %u %u %u", user, nice, system, idle);
	fclose(fp_stat);
}

int main(int argc, char **argv) {
 	char result[256];
 	char ip[16];
 	int port;
	unsigned int user1, nice1, system1, idle1;
	unsigned int user2, nice2, system2, idle2;
	float puser, psystem, pnice, pidle, total;

 	if(argc < 2) {
 		return 1;
 	}
 
 	sscanf(argv[1], "%[^:]:%d", &ip, &port);
 
	getvalues(&user1, &nice1, &system1, &idle1);
//	printf("dump: %u %u %u %u\n", user1, &nice1, nice1, idle1); 
	sleep(60);
	getvalues(&user2, &nice2, &system2, &idle2);
//	printf("dump: %u %u %u %u\n", user2, nice2, system2, idle2); 

	total = ( user2 + system2 + nice2 + idle2 ) -
		( user1 + system1 + nice1 + idle1 );

	puser = ( user2 - user1 ) / total * 100;
	pnice = ( nice2 - nice1 ) / total * 100;
	psystem = ( system2 - system1 ) / total * 100;
	pidle = ( idle2 - idle1 ) / total * 100;

	sprintf(result, "%s N:%f:%f:%f:%f", argv[2], puser, pnice, psystem, pidle);
	printf("%s\n", result);
	if(udpsend(ip, port, result) != 0)
		return 3;
	return 0;
}
