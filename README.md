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

2. If END character is present, send ESC followed by ESC_END instead.

3. Append an END character at the end of frame.



## Receiver

When receiving data, the receiver will process the data according to the following steps:

1. If an ESC character followed by an ESC_ESC character is received, it will be parsed as receiving one ESC character.

2. If an ESC character followed by an ESC_END character is received, it will be parsed as receiving one END character.

3. If an END character is received, then the frame is finished. The receiver can proceed to process the data received or continue to receive the following frames.



## Pros / Cons

- The Protocol needs to run under 8-bit data mode. (Will not work under 7-bit, 5-bit or other data modes).

- The Protocol does not provide data validation. The integrity of data will be ensured by the upper protocols.

- The END character is the newline character in ASCII character set, and frames will be separated into different lines in a serial monitor. This is easier to debug and more readable compared to SLIP and COBS.



## Python Example

```python
import struct
import logging
import time

import serial

logger = logging.getLogger("NLSMSerial")
logger.setLevel(logging.INFO)

log_handler = logging.StreamHandler()
log_handler.setLevel(logging.INFO)
log_handler.setFormatter(logging.Formatter('%(asctime)s [%(name)s] %(levelname)s: %(message)s'))
logger.addHandler(log_handler)


class NLSMSerial:
    END = b"\x0A"
    ESC = b"\x0B"
    ESC_END = b"\x1A"
    ESC_ESC = b"\x1B"
    
    def __init__(self, port, baudrate=115200, timeout=0, persistent=True):
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.persistent = persistent

        self.connect()

    def connect(self):
        self._ser = None
        while not self._ser:
            try:
                self._ser = serial.Serial(self.port, baudrate=self.baudrate, timeout=self.timeout)
            except serial.serialutil.SerialException as e:
                if not self.persistent:
                    raise e
                logger.info("Connecting to \"{0}\"...".format(self.port))
                time.sleep(1)
        
        logger.info("\"{0}\" Connected.".format(self.port))

    def _read(self):  
        try:
            c = self._ser.read(1)
        except serial.serialutil.SerialException:
            if not self.persistent:
                raise e
            logger.warning("Connection lost. Reconencting...")
            self.connect()
        return c
    
    def _write(self, c):
        try:
            self._ser.write(c)
        except serial.serialutil.SerialException:
            if not self.persistent:
                raise e
            logger.warning("Connection lost. Reconencting...")
            self.connect()
        return True

    def receive(self):
        c = b""
        buffer = b""

        while c != self.END:
            if c == self.ESC:
                c = self._read()
                if c == self.ESC_END:
                    buffer += self.END
                elif c == self.ESC_ESC:
                    buffer += self.ESC
                else:
                    buffer += c
            else:
                buffer += c
            
            c = self._read()
            if c == b"":
                return b""  # timeout
        
        return buffer
    
    def transmit(self, buffer):
        index = 0

        while index < len(buffer):
            c = struct.pack("B", buffer[index])
            if c == self.END:
                self._write(self.ESC)
                self._write(self.ESC_END)
            elif c == self.ESC:
                self._write(self.ESC)
                self._write(self.ESC_ESC)
            else:
                self._write(c)
            index += 1
            
        self._write(self.END)
        
        return index

    def flushRX(self):
        is_empty = True
        
        while self._ser.read():
            is_empty = False
            
        return is_empty

# ============================================================== #


import time
import random
import logging



ser = NLSMSerial("COM20", 115200, timeout=0.1)



datasize = 0
i = 0

performance_prev_t = time.time()
while True:
    t = time.time()
    buf = "%s\n" % t
    
    ser.transmit(buf.encode())

    time.sleep(0.01)

    buf = ser.receive()

    #ser.flushRX()
    
    datasize += len(buf)

    i += 1
    
    try:
        prev_t = float(buf.decode().replace("\n", ""))
        print(t, "\t", time.time() - prev_t)
    except KeyboardInterrupt as e:
        raise e
    except:
        print(buf)
        pass


    if i > 100:
        print(datasize / (time.time() - performance_prev_t), " wd/s")
        i = 0
        datasize = 0
        performance_prev_t = time.time()



```
