all: memory df iftraff nproc loadavg nconn cpumhz ifpackets sendmail-mailq cpu nagios_check_rrd_last wireless-snr

memory: memory.c udpsend.c
	gcc -O2 -o memory memory.c udpsend.c

df: df.c udpsend.c
	gcc -O2 -o df df.c udpsend.c

iftraff: iftraff.c udpsend.c
	gcc -O2 -o iftraff iftraff.c udpsend.c

nproc: nproc.c udpsend.c
	gcc -O2 -o nproc nproc.c udpsend.c

loadavg: loadavg.c udpsend.c
	gcc -O2 -o loadavg loadavg.c udpsend.c

nconn: nconn.c udpsend.c
	gcc -O2 -o nconn nconn.c udpsend.c

cpumhz: cpumhz.c udpsend.c
	gcc -O2 -o cpumhz cpumhz.c udpsend.c

ifpackets: ifpackets.c udpsend.c
	gcc -O2 -o ifpackets ifpackets.c udpsend.c

sendmail-mailq: sendmail-mailq.c udpsend.c
	gcc -O2 -o sendmail-mailq sendmail-mailq.c udpsend.c

cpu: cpu.c udpsend.c
	gcc -O2 -o cpu cpu.c udpsend.c

wireless-snr: wireless-snr.c udpsend.c
	gcc -O2 -o wireless-snr wireless-snr.c udpsend.c

nagios_check_rrd_last: nagios_check_rrd_last.c
	gcc -O2 -lrrd -o nagios_check_rrd_last nagios_check_rrd_last.c

clean:
	rm -f memory df iftraff nproc loadavg nconn cpumhz ifpackets sendmail-mailq cpu wireless-snr nagios_check_rrd_last
