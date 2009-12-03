OROCOS_TARGET = gnulinux
OROPATH = /usr/local
CXX = g++
LD = ld

OROSERVFLAGS= -Wall -g `PKG_CONFIG_PATH=${OROPATH}/lib/pkgconfig pkg-config orocos-rtt-corba-${OROCOS_TARGET} --cflags`
OROSERVLIBS = `PKG_CONFIG_PATH=${OROPATH}/lib/pkgconfig pkg-config orocos-rtt-corba-${OROCOS_TARGET} --libs`

OROFLAGS= -Wall -g `PKG_CONFIG_PATH=${OROPATH}/lib/pkgconfig pkg-config orocos-ocl-${OROCOS_TARGET} orocos-rtt-corba-${OROCOS_TARGET} --cflags`
OROLIBS = `PKG_CONFIG_PATH=${OROPATH}/lib/pkgconfig pkg-config orocos-ocl-${OROCOS_TARGET} orocos-rtt-corba-${OROCOS_TARGET} --libs`

all: eserver eclient eprocess smallnet

smallnet: SmallNetwork.cpp
	${CXX} -o smallnet ${OROSERVFLAGS} SmallNetwork.cpp ${OROSERVLIBS}

eserver: ExecutionServer.o
	${CXX} -o eserver ExecutionServer.o  ${OROSERVLIBS}

ExecutionServer.o: ExecutionServer.cxx ExecutionServer.hpp
	${CXX} ${OROSERVFLAGS} -c ExecutionServer.cxx -o ExecutionServer.o

eprocess: SingleProcess.o
	${CXX} -o eprocess SingleProcess.o  ${OROLIBS}

SingleProcess.o: SingleProcess.cxx ExecutionServer.hpp
	${CXX} ${OROFLAGS} -c SingleProcess.cxx -o SingleProcess.o


eclient: ExecutionClient.o
	${CXX} -o eclient ExecutionClient.o  ${OROLIBS}

ExecutionClient.o: ExecutionClient.cxx 
	${CXX} ${OROFLAGS} -c ExecutionClient.cxx -o ExecutionClient.o

eclientany: ExecutionClientAny.o
	${CXX} -o eclientany ExecutionClientAny.o  ${OROLIBS}

ExecutionClientAny.o: ExecutionClientAny.cxx 
	${CXX} ${OROFLAGS} -c ExecutionClientAny.cxx -o ExecutionClientAny.o

clean:
	rm -f *.o
	rm -f eclient eserver eprocess eclientany smallnet
