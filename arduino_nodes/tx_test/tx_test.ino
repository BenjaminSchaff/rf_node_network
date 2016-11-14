#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"


// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9, 10);

// Addresses of the 2 nodes. 5 bytes each
const uint64_t root_addr = 0xF0F0F0F0AA;
const uint64_t node_addr = 0xF0F0F0F0A1;

void setup(void)
{
  Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24/examples/pingpair/\n\r");


  // Setup and configure rf radio
  radio.begin();


  radio.setRetries(15, 15); // retries 15 times after 15*250us each

  // optionally, reduce the payload size.  seems to improve reliability
  radio.setPayloadSize(8);

  // Open pipes to other nodes for communication
  radio.openWritingPipe(root_addr); //we are root, write from here
  radio.openReadingPipe(1, node_addr); //read from node

  radio.startListening();

  // print config for debug
  radio.printDetails();
}

void loop(void)
{
  // First, stop listening so we can talk.
  radio.stopListening();

  // Take the time, and send it.  This will block until complete
  unsigned long time = millis();
  Serial.print("Now sending ");
  Serial.print(time);
  bool ok = radio.write( &time, sizeof(unsigned long) );

  if (ok)
    Serial.print(" ok...");
  else
    Serial.print(" failed.\n\r");

  // Now, continue listening
  radio.startListening();

  // Wait here until we get a response, or timeout (250ms)
  unsigned long started_waiting_at = millis();
  bool timeout = false;
  while ( ! radio.available() && ! timeout )
    if (millis() - started_waiting_at > 200 )
      timeout = true;

  // Describe the results
  if ( timeout )
  {
      Serial.print("Failed, response timed out.\r\n");
      delay(50);
    }
  else
  {
  
      unsigned long payload;
      radio.read( &payload, sizeof(unsigned long) );
  
      // print revieved message
      Serial.print("Got response ");
      Serial.println(payload);
  
      delay(2000);
    }
}
