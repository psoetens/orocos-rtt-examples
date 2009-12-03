OROCOS_TARGET = gnulinux
OROPATH = /usr/local
CC = g++
LD = ld

OROFLAGS= -Wall -g -fPIC -DOCL_DLL_EXPORT `PKG_CONFIG_PATH=${OROPATH}/lib/pkgconfig pkg-config orocos-ocl-${OROCOS_TARGET} orocos-rtt-${OROCOS_TARGET} --cflags`
OROLIBS = `PKG_CONFIG_PATH=${OROPATH}/lib/pkgconfig pkg-config orocos-rtt-${OROCOS_TARGET} --libs`

all: libplant.so libcontroller.so

libplant.so: Plant.cpp
	${CC} ${OROFLAGS} $< -o $@ -shared ${OROLIBS}

libcontroller.so: Controller.cpp
	${CC} ${OROFLAGS} $< -o $@ -shared ${OROLIBS}

clean:
	rm -f *.o *.idl orocos.log *.so

FILES= controller-program.ops README Plant.cpp Controller.cpp Makefile example.cpf

dist:
	cd .. ;\
	tar -cvzf deployment.tar.gz $(addprefix deployment/,$(FILES))
