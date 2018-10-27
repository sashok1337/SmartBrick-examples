#include <SPI.h>
#include <RF24.h>

#define LEDPIN 13
#define CE 5
#define CSN 6
RF24 radio(CE, CSN);

//#define ROLE_RECEIVER 1
#define RETRANSMIT_WAIT_TIME 0
#define RETRANSMIT_ATTEMPTS 1

int packetCount = 0;
#define package_size sizeof(packetCount)

const uint64_t pipes[2] = {0xABCDABCD71LL, 0x544d52687CLL};

void setup() {
  Serial.begin(9600);
  
  radio.begin();

  radio.setAutoAck(1);
  radio.setDataRate(RF24_250KBPS);
  radio.setCRCLength(RF24_CRC_16);
  radio.setPALevel(RF24_PA_MAX);
  radio.enableDynamicPayloads();
  radio.setRetries(RETRANSMIT_WAIT_TIME, RETRANSMIT_ATTEMPTS);

  radio.setChannel(110);

#ifdef ROLE_RECEIVER
  radio.openReadingPipe(1, pipes[0]);
  radio.startListening();
#else
  radio.openReadingPipe(1, pipes[1]);
  radio.openWritingPipe(pipes[0]);
#endif
}

void loop() {
#ifdef ROLE_RECEIVER
  if (radio.available()) {
    short rsize = radio.getDynamicPayloadSize();
    if (rsize < 1) {
      return;
    }
    if (rsize == package_size) {
      radio.read(&packetCount, package_size);

      digitalWrite(LEDPIN, HIGH);
      delay(100);
      digitalWrite(LEDPIN, LOW);

      Serial.println(packetCount);
    } else {
      radio.flush_rx();
    }
  }
#else
  radio.write(&packetCount, package_size);
  ++packetCount;
  delay(200);
#endif
}
