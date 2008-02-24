#include <rrd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv) {
	char **rrdargs;
	char rrdvalues[512];
	time_t tlast, tnow;
	int tdiff;

	rrdargs = malloc(2 * sizeof(char *));
	rrdargs[0] = malloc(4);   /* last      */
	rrdargs[1] = malloc(256); /* file name */

	*rrdargs[0] = '\0'; *rrdargs[1] = '\0';
	strcpy(rrdargs[0], "last"); 
	strcpy(rrdargs[1], argv[2]); 
	rrd_clear_error();
	tlast = rrd_last(2, rrdargs);
	if(rrd_test_error() == 1) {
		printf("Unknown: error from rrd: %s\n", rrd_get_error());
		exit(3);	/* Unknown */
	}

	tnow = time(&tnow);
	tdiff = tnow - tlast;

	if(tdiff > atoi(argv[1])) {
		printf("Critical: %d seconds since last update.\n", tdiff);
		exit(2); /* Critical */
	}
	else {
		printf("OK: %d seconds since last update.\n", tdiff);
		exit(0); /* OK */
	}
	exit(3);	/* Unknown */

}


