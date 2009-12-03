
OROCOS_TARGET = gnulinux
OROPATH = /usr/local
CC = g++
LD = ld

OROFLAGS= -Wall -g `PKG_CONFIG_PATH=${OROPATH}/lib/pkgconfig pkg-config orocos-ocl-${OROCOS_TARGET} orocos-rtt-${OROCOS_TARGET} --cflags`
OROLIBS = `PKG_CONFIG_PATH=${OROPATH}/lib/pkgconfig pkg-config orocos-ocl-${OROCOS_TARGET} orocos-rtt-${OROCOS_TARGET} --libs`

all: simpletask

simpletask.o: SimpleTask.cxx ${OROPATH}/include/rtt/os/fosi.h
	${CC} -c ${OROFLAGS} $< -o $@

simpletask: simpletask.o
	${CC} -o $@ $@.o ${OROLIBS}


clean:
	rm -f *.o
	rm -f simpletask

FILES= README SimpleTask.cxx Makefile
dist:
	cd .. ;\
	tar -cvzf simple-task.tar.gz $(addprefix task-intro/,$(FILES))
