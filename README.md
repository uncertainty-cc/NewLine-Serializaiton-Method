# NewLine-Serializaiton-Method

The **NewLine Serialization Method**, or the NewLine Serialization Protocol, is a minimalistic communication protocol designed for various serial communication scenarios. It defines how the serial data stream is separated into individual packets. Due to its simplicity, it is most suited in embedded systems. 



## Special Characters

There are 4 (four) different characters defined in the ISLP specification. 

| HEX  | DEC | OCT  | Name    | Description                    |
| ---- | --- | ---- | ------- | ------------------------------ |
| 0x0A | 10  | 0o12 | END     | End Of Frame character         |
| 0x0B | 11  | 0o13 | ESC     | Escape character               |
| 0x1A | 26  | 0o32 | ESC_END | Escaped End Of Frame character |
| 0x1B | 27  | 0o33 | ESC_ESC | Escaped Escape character       |



## Sender

When transmitting data, the sender will pre-process the data according to the following steps:

1. If ESC character is present, send ESC followed by ESC_ESC instead. 

```
[... | x | ESC | x | ...] => [... | x | ESC | ESC_ESC | x | ...]
```

2. If END character is present, send ESC followed by ESC_END instead.

```
[... | x | END | x | ...] => [... | x | ESC | ESC_END | x | ...]
```

3. Append an END character at the end of frame.

```
[... | x ] => [... | x | END ]
```


## Receiver

When receiving data, the receiver will process the data according to the following steps:

1. If an ESC character followed by an ESC_ESC character is received, it will be parsed as receiving one ESC character.

```
[... | x | ESC | ESC_ESC | x | ...] => [... | x | ESC | x | ...]
```

2. If an ESC character followed by an ESC_END character is received, it will be parsed as receiving one END character.

```
[... | x | ESC | ESC_END | x | ...] => [... | x | END | x | ...]
```

3. If an END character is received, then the frame is finished. The receiver can proceed to process the data received or continue to receive the following frames.

```
[... | x | END ] => [... | x ]
```

## Pros / Cons

- The Protocol needs to run under 8-bit data mode. (Will not work under 7-bit, 5-bit or other data modes).

- The Protocol does not provide data validation. The integrity of data will be ensured by the upper protocols.

- The END character is the newline character in ASCII character set, and frames will be separated into different lines in a serial monitor. This is easier to debug and more readable compared to SLIP and COBS.

## Arduino

### Installation

Download the `.zip` file and extract it into `<Arduino_installation_path>/Sketchbook/libraries/`

### Usage

A simple echo program

```cpp
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

```

## Python

### Installation

```bash
pip install dotserializer
```

### Usage

List all available ports

```python
from dotserializer import Serializer

ports = Serializer.getAvailablePorts()
print(ports)
```

A simple echo program

```python
import time

from dotserializer import Serializer

ser = Serializer(port="COM1", baudrate=115200)

# wait for serial device to initialize
time.sleep(2)

while True:
    buffer = ser.receive()
    print("recv", buffer)
    ser.transmit(buffer)

```

Packet receiving timeout, blocking vs non-blocking

```python
buffer = ser.receive(timeout=None)  # blocking (default)
buffer = ser.receive(timeout=0)     # non-blocking, return b"" if no data
buffer = ser.receive(timeout=1)     # timeout for 1 sec, return b"" if no data
```