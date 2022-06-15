#include "dotserializer.h"

using namespace rath;

uint8_t buffer[128];

void setup() {
  Serializer.init(115200);
}

void loop() {
  uint16_t rx_size = Serializer.receive(buffer, 128);

  Serializer.transmit(buffer, rx_size);
}
