TARGET=gnulinux
OROPATH=/usr/local
CXXFLAGS=-I${OROPATH}/include -DOROCOS_TARGET=${TARGET} -Wall
LDFLAGS=-L${OROPATH}/lib -Wl,-rpath ${OROPATH}/lib -lorocos-taskbrowser-${TARGET} -lorocos-rtt-${TARGET} -lpthread -lrt

all: helloworld

helloworld: HelloWorld.cpp
	$(CXX) -g -o helloworld HelloWorld.cpp $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f helloworld orocos.log .tb_history