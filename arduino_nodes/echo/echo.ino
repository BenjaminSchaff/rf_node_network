/*
 * Simple test node, echos and recieved messages back to sender.	Prints to serial for debug
 */
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define SIG_TURN_ON  0x000000FF
#define SIG_TURN_OFF 0x0000FF00
#define SIG_ACKNO    0x00FF0000

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 for CE and CSN
RF24 radio(9, 10);

// Addresses of the 2 nodes. 5 bytes each
const uint64_t root_addr = 0xF0F0F0F0AA;
const uint64_t node_addr = 0xF0F0F0F0A1;

void setup(void)
{
	Serial.begin(9600);
	Serial.print("\n\rRF24 echo test\n\r");

	radio.begin();
	radio.setRetries(15, 15); //15 retries after 15*250us each
	radio.setPayloadSize(8); // Reduce the payload size.	seems to improve reliability

	radio.openWritingPipe(node_addr); //we are remote node
	radio.openReadingPipe(1, root_addr); //read from root

	radio.startListening();
}

void loop(void)
{
	if (radio.available())	// if there is data ready
	{
		unsigned long payload;
		bool done = false;
		while (!done)
		{
			// Fetch the payload, and see if this was the last one.
			done = radio.read( &payload, sizeof(unsigned long) );

			//print to serial for debug
			Serial.print("Got: ");
			Serial.print(payload, HEX);
			
			// wait for other to switch modes
			delay(20);
		}

		// stop rx before tx
		radio.stopListening();

		if (payload == SIG_TURN_ON) {
			//digitalWrite(LED_BUILTIN, HIGH);
			payload = SIG_ACKNO;
		} else if (payload == SIG_TURN_OFF) {
			//digitalWrite(LED_BUILTIN, LOW);
			payload = SIG_ACKNO;
		}
		// echo payload
		radio.write( &payload, sizeof(unsigned long) );
		Serial.print(" Sent response.\n\r");

		// back to rx mode
		radio.startListening();
	}
}

