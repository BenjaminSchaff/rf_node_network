prefix := /usr/local

# The recommended compiler flags for the Raspberry Pi
CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s

# define all programs
PROGRAMS = tx_test
SOURCES = ${PROGRAMS:=.cpp}

all: ${PROGRAMS}

${PROGRAMS}: ${SOURCES}
	g++ ${CCFLAGS} -Wall -I./RF24/RPi/RF24/ -lrf24-bcm $@.cpp -o $@

clean:
	rm -rf $(PROGRAMS)

