/*
 * Receives a command from root node to turn light switch on or off using servo
 */
#include <SPI.h>
#include <Servo.h>
#include "nRF24L01.h"
#include "RF24.h"

#define SIG_TURN_ON  0x000000FF
#define SIG_TURN_OFF 0x0000FF00
#define SIG_ACKNO    0x00FF0000

#define SERVO_ON 160 // servo angle to flip switch on
#define SERVO_OFF 60 // angle to flip switch off
#define SERVO_MID 95 // idle pos of servo]

#define DISABLE_DELAY 1500
#define SERVO_PIN 8
// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 for CE and CSN
RF24 radio(9, 10);
Servo servo;

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

	// print config for debug
	//radio.printDetails(); //does nothing with stdout defined

	//pinMode(LED_BUILTIN, OUTPUT); //dont use this pin, its used for SCK
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

			delay(20);

		}

		// wait for other to switch modes
		//delay(20);
		// stop rx before tx
		//radio.stopListening();

		if (payload == SIG_TURN_ON) {
			set_switch(1);
			//payload = SIG_ACKNO;
		} else if (payload == SIG_TURN_OFF) {
			set_switch(0);
			//payload = SIG_ACKNO;
		}

		// back to rx mode
		//radio.startListening();
	}
}

/*
 * 
 */
void set_switch(int i) {
	servo.attach(SERVO_PIN);//enable pwm on servo pin
	servo.write(i?SERVO_ON:SERVO_OFF);	// tell servo to go to move
	delay(DISABLE_DELAY);	// wait for servo to get there
	servo.write(SERVO_MID);	// return servo to idle state in middle
	delay(DISABLE_DELAY/2);	// let it get there
	servo.detach();			// disable pwm (and thus motor) to save power
}
