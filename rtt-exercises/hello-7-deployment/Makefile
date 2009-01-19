TARGET=gnulinux
OROPATH=/usr/local
CXXFLAGS=-I${OROPATH}/include -DOROCOS_TARGET=${TARGET} -fPIC -DOCL_DLL_EXPORT
LDFLAGS=-shared -L${OROPATH}/lib -Wl,-rpath ${OROPATH}/lib -lorocos-rtt-${TARGET}

all: libhello-${TARGET}.so libworld-${TARGET}.so

libhello-${TARGET}.so: Hello.cpp
	$(CXX) -g -o libhello-${TARGET}.so Hello.cpp $(CXXFLAGS) $(LDFLAGS)

libworld-${TARGET}.so: World.cpp
	$(CXX) -g -o libworld-${TARGET}.so World.cpp $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f libhello-${TARGET}.so libworld-${TARGET}.so orocos.log .tb_history