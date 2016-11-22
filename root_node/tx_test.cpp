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

	cout << "RF24/examples/GettingStarted/\n";

	radio.begin();

	// optionally, increase the delay between retries & # of retries
	radio.setRetries(15,15); // retries 15 times after 15*250us each
	radio.setPayloadSize(8); // drop payload size to 8 bytes for reliability

	radio.openWritingPipe(root_addr);
	radio.openReadingPipe(1, node_addr);

	// Dump the configuration of the rf unit for debugging
	radio.printDetails();

	//radio.startListening();
	
	while (1)
	{
		// End rx mode before tx.
		radio.stopListening();

		//unsigned long time = millis();
		unsigned long message = (i++)%2?SIG_TURN_ON:SIG_TURN_OFF;
		printf("Sending\t%lx ", message);
		bool success = radio.write( &message, sizeof(unsigned long) );

		if (!success){
			printf(" failed.\n");
		} else {
			printf(" success. \t");
			
			// resume rx, wait for response
			radio.startListening();

			// Wait for response or timeout
			unsigned long wait_start = millis();
			bool timeout = false;
			while ( !radio.available() && !timeout ) {
				if (millis() - wait_start > 2500 )
					timeout = true;
			}


			// Print results of rx
			if (timeout) {
				printf("No response, timed out.\n");
			} else {
				unsigned long payload;
				radio.read( &payload, sizeof(unsigned long) );

				// print recieved message
				printf("Got response %lx\n", payload);
			}
		}
		sleep(1);
	} 
	// loops forever, doesnt get here
	return 0;
}

