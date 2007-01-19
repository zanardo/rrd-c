all:
	@echo ";; Creating new rrd databases defined in ./conf-data"
	./rrd-create-data

me:
	gcc -O2 -o rrd -lrrd rrd.c 

clean:
	rm -f rrd
