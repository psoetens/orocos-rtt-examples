OROCOS_TARGET = gnulinux
OROPATH = /usr/local
CXX = g++
LD = ld

OROFLAGS= -Wall -g `PKG_CONFIG_PATH=${OROPATH}/lib/pkgconfig pkg-config orocos-rtt-${OROCOS_TARGET} --cflags`
OROLIBS = `PKG_CONFIG_PATH=${OROPATH}/lib/pkgconfig pkg-config orocos-rtt-${OROCOS_TARGET} --libs`

all: EmbeddedTask

EmbeddedTask: EmbeddedTask.cpp
	${CXX} -O3  EmbeddedTask.cpp -o EmbeddedTask ${OROFLAGS} ${OROLIBS}

#-fno-exceptions

clean:
	rm -f *.o EmbeddedTask orocos.log
