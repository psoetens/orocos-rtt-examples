
OROCOS_TARGET = gnulinux
OROPATH = /usr/local
CC = g++
LD = ld

OROFLAGS= -Wall -g `PKG_CONFIG_PATH=${OROPATH}/lib/pkgconfig pkg-config orocos-ocl-${OROCOS_TARGET} orocos-rtt-${OROCOS_TARGET} --cflags`
OROLIBS = `PKG_CONFIG_PATH=${OROPATH}/lib/pkgconfig pkg-config orocos-ocl-${OROCOS_TARGET} orocos-rtt-${OROCOS_TARGET} --libs`

all: taskstate

taskstate.o: TaskState.cpp ${OROPATH}/include/rtt/os/fosi.h ${OROPATH}/include/ocl/TaskBrowser.hpp
	${CC} -c ${OROFLAGS} $< -o $@

taskstate: taskstate.o
	${CC} -o $@ $@.o ${OROLIBS}


clean:
	rm -f *.o
	rm -f taskstate

FILES= README TaskState.cxx TaskState.cpf Makefile
dist:
	cd .. ;\
	tar -cvzf task-state.tar.gz $(addprefix taskstate/,$(FILES))
