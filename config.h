/* 
   Where rrd should bind it's UDP server.  By default it attaches
   to 0.0.0.0, which means all network interfaces. You can change
   this to another IP.
*/
#define RRDHOST "0.0.0.0"

/* 
   Which UDP port should rrd listen for packets. The default port
   is 23456. 
*/
#define RRDPORT 23456

/* 
   System user that rrd should be run. If rrd is started by root,
   it will change it's process to the user below. Default is 
   nobody.
*/
#define RRDUSER "nobody"

/*
   Where rrd is installed. Default is /opt/rrd.
*/
#define RRDROOT "/opt/rrd"
