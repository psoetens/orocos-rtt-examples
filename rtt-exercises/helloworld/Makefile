TARGET=gnulinux
OROPATH=/opt/2.0
CXXFLAGS=-I${OROPATH}/include -DOROCOS_TARGET=${TARGET} -Wall
LDFLAGS=-L${OROPATH}/lib -Wl,-rpath ${OROPATH}/lib -lorocos-taskbrowser-${TARGET} -lorocos-rtt-${TARGET} -lrt -lpthread

all: helloworld

helloworld: HelloWorld.cpp
	$(CXX) -g -o helloworld HelloWorld.cpp $(CXXFLAGS) $(LDFLAGS)
	@echo "success."

clean:
	rm -f helloworld orocos.log .tb_history
	@echo "clean."
