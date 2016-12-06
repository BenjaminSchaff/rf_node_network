/**
 * Basic transmitter test for RF24 module on RPi.  remote node is an Arduino running echo 
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <RF24/RF24.h>

using namespace std;

// RPi generic:
RF24 radio(22,0);

// Addresses of the 2 nodes. 5 bytes each
const uint64_t root_addr = 0xF0F0F0F0AA; // the root node (this one)
const uint64_t node_addr = 0xF0F0F0F0A1; // the remote node

#define SIG_TURN_ON  0x000000FF
#define SIG_TURN_OFF 0x0000FF00
#define SIG_ACKNO   0x00FF0000

int i = 0; // because what program doesnt have an int i?


int main(int argc, char** argv){
	unsigned long message = 0;
	int retry = 5;
	bool on = false; // state of light (on or off)

	// check to make sure there is the right amount of arguments
	if (argc < 2) {
		printf("Need one argument only");
		return -1;
	}
	radio.begin();

	// optionally, increase the delay between retries & # of retries
	radio.setRetries(15,15); // retries 15 times after 15*250us each
	radio.setPayloadSize(8); // drop payload size to 8 bytes for reliability

	radio.openWritingPipe(root_addr);
	radio.openReadingPipe(1, node_addr);

	// End rx mode before tx.
	radio.stopListening();

	// parse input argument
	if (!strcmp(argv[1],"1")) {
		on = true;
	} else {
		on = false;
	}

	// TODO

	while (retry > 0) {
		if (on)
			message = SIG_TURN_ON;
		else {
			message = SIG_TURN_OFF;
		}
		//printf("Sending\t%lx ", message);
		bool success = radio.write( &message, sizeof(unsigned long) );
		
		if (!success){
			printf(" failed.\n");
			retry--;
			usleep(500000); // wait .5 sec before retrying
		} else {
			printf(" success. \n");
			retry = 0;
		
		}
	} 
	return 0;
}

