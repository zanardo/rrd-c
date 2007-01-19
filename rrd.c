/* Copyright (c) 2006, J. A. Zanardo Jr. <zanardo@gmail.com>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include "config.h"

#include <netinet/in.h>
#include <pwd.h>
#include <rrd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define VERSION "0.2"

int main(void) {
	int sockfd, bytesrecv;
	struct sockaddr_in server, client;
	socklen_t len;
	char msg[1024];
	char *clientip;
	char **rrdargs;
	char rrdfile[256];
	char rrdvalues[512];
	char *c;
	char datapath[sizeof(RRDROOT) + 6];
	struct passwd *userinfo;

	/* rrdtool arguments */
	rrdargs = malloc(3 * sizeof(char *));
	rrdargs[0] = malloc(7);   /* update    */
	rrdargs[1] = malloc(256); /* file name */
	rrdargs[2] = malloc(512); /* values    */

	/* flushes the stdout buffer at every line */
	setlinebuf(stdout);

	/* constructing data path */
	strcpy(datapath, RRDROOT);
	strcat(datapath, "/data");

    userinfo = getpwnam(RRDUSER);
    if(userinfo == NULL) {
        printf("error: could not read user '%s' information\n", RRDUSER);
        exit(1);
    }

	if(chdir(datapath) == -1) {
		printf("error: could not chdir() to %s\n", datapath);
		exit(1);
	}

	/* chrooting to data dir and dropping privileges if we are running as root */
	if(getuid() == (uid_t)0) {
		if(chroot(".")) {
			printf("could not chroot() to %s!\n", datapath);
			exit(1);
		}
		printf("got into chroot() jail to %s\n", datapath);

		if(setuid(userinfo->pw_uid)) {
			printf("error dropping root privileges to %s!\n", RRDUSER);
			exit(1);
		}
		printf("dropped root privileges to '%s'\n", RRDUSER);
	}
	else
		printf("warning: not running as root, so we can't chroot()\n");

	printf("rrd daemon version %s starting\n", VERSION);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1) {
		printf("an strange error has occurred creating socket file descriptor\nmaybe you are out of file descriptors?\n");
		exit(1);
	}
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(RRDHOST);
	server.sin_port = htons(RRDPORT);
	if(bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
		printf("could not bind to %s port %d\n", RRDHOST, RRDPORT);
		exit(1);
	}

	printf("listening on %s:%d\n", RRDHOST, RRDPORT);

	printf("rrd daemon is ready to receive requests\n");

	for (;;) {
		bzero(msg, sizeof(msg));
		len = sizeof(client);
		bytesrecv = recvfrom(sockfd, msg, (size_t)sizeof(msg), 0, (struct sockaddr *)&client,&len);
		clientip = (char*)inet_ntoa(client.sin_addr);
		rrdfile[0] = rrdvalues[0] = '\0';
		sscanf(msg, "%100[a-z0-9-] %100[N:.0-9U-] ", rrdfile, rrdvalues);
		c = msg;
		while(*c != '\0') {		/* chomp */
			if(*c == '\n' || *c == '\r') 
				*c = ' ';
			c++;
		}
		if(rrdfile[0] == '\0' || rrdvalues[0] == '\0') 
			printf("[%s] invalid packet (%d bytes)\n", clientip, bytesrecv);
		else {
			/* updating rrd */
			printf("[%s] %s\n", clientip, msg);
			*rrdargs[0] = '\0'; *rrdargs[1] = '\0'; *rrdargs[2] = '\0';
			strcpy(rrdargs[0], "update"); 
			strcpy(rrdargs[1], rrdfile); 
			strcat(rrdargs[1], ".rrd");
			strcpy(rrdargs[2], rrdvalues); 
			rrd_clear_error();
			rrd_update(3, rrdargs);
			if(rrd_test_error() == 1) 
				printf("error from rrd: %s\n", rrd_get_error());
		}
	}
}
