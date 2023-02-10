from cc.serializer import UARTSerializer

ser = Serializer(port="COM1", baudrate=115200)

while True:
    buffer = ser.receive()
    print("recv:", buffer)
    ser.transmit(buffer)
