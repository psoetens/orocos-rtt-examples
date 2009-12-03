OROCOS_TARGET = gnulinux
OROPATH = /usr/local
CC = g++
LD = ld

OROFLAGS= -Wall -g `PKG_CONFIG_PATH=${OROPATH}/lib/pkgconfig pkg-config orocos-ocl-${OROCOS_TARGET} orocos-rtt-${OROCOS_TARGET} --cflags`
OROLIBS = `PKG_CONFIG_PATH=${OROPATH}/lib/pkgconfig pkg-config orocos-ocl-${OROCOS_TARGET} orocos-rtt-${OROCOS_TARGET} --libs`

all: taskintro

taskintro.o: TaskIntro.cxx ${OROPATH}/include/rtt/os/fosi.h *.hpp
	${CC} -c ${OROFLAGS} $< -o $@

taskintro: taskintro.o
	${CC} -o $@ $@.o ${OROLIBS}

clean:
	rm -f *.o *.idl orocos.log
	rm -f taskintro

FILES= CountingSM.osd README TaskIntro.cxx cpf.dtd Makefile PeriodicTaskContext.hpp ReactiveTaskContext.hpp WaitingTaskContext.hpp program.ops
dist:
	cd .. ;\
	tar -cvzf task-intro.tar.gz $(addprefix task-intro/,$(FILES))
