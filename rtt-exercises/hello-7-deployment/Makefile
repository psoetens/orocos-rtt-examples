TARGET=gnulinux
OROPATH=/opt/2.0
CXXFLAGS=-I${OROPATH}/include -DOROCOS_TARGET=${TARGET} -fPIC -DRTT_COMPONENT
LDFLAGS=-shared -L${OROPATH}/lib -Wl,-rpath ${OROPATH}/lib -lorocos-rtt-${TARGET}

all: libhello-${TARGET}.so libworld-${TARGET}.so
	@echo "success."

libhello-${TARGET}.so: Hello.cpp
	$(CXX) -g -o libhello-${TARGET}.so Hello.cpp $(CXXFLAGS) $(LDFLAGS)

libworld-${TARGET}.so: World.cpp
	$(CXX) -g -o libworld-${TARGET}.so World.cpp $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f libhello-${TARGET}.so libworld-${TARGET}.so orocos.log .tb_history
	@echo "clean."