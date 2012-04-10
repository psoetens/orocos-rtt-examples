# This Makefile is here for 'rosmake' like systems. In case you don't use
# ROS, it will create a build directory for you and build the package with
# default settings. It will install in the current directory.
ifdef ROS_ROOT
include $(shell rospack find mk)/cmake.mk
else
$(warning This Makefile builds this package with default settings)
all:
	mkdir -p build
	cd build ; cmake .. -DINSTALL_PATH=$(pwd) && make
	echo -e "\n Now do 'make install' to install this package.\n"
install: all
	cd build ; make install
endif