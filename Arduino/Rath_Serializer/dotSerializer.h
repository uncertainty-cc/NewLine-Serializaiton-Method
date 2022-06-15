/**
 * @file dotSerializer.h
 * @author Rath Robotics
 * @brief Transmit and receive Serial data and divide the stream into packets with NLSM protocol.
 * @version 1.0
 * @date 2022-06-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "Arduino.h"

namespace rath {
/**
 * NewLine Serialization Method characters definition
 * https://sites.google.com/view/nlsm-spec/home
 */
#define NLSM_END 0x0AU
#define NLSM_ESC 0x0BU
#define NLSM_ESC_END 0x1AU
#define NLSM_ESC_ESC 0x1BU

class SerializerClass {
 public:
  /**
   * Initialize the serial object.
   *
   * @param baudrate: the baudrate of the serial port.
   */
  void init(int64_t baudrate) {
    Serial.begin(baudrate);
  }

  /**
   * @param buffer: the pointer to the data buffer to store incoming
   * data.
   * @param size: the maximum number of bytes to receive.
   */
  uint16_t receive(uint8_t *buffer, uint16_t size) {
    uint8_t c;
    uint16_t index = 0;

    while (!Serial.available()) {}

    c = Serial.read();

    while (c != NLSM_END) {
      if (c == NLSM_ESC) {
        while (!Serial.available()) {}

        c = Serial.read();

        if (c == NLSM_ESC_END) {
          buffer[index] = NLSM_END;
        }
        else if (c == NLSM_ESC_ESC) {
          buffer[index] = NLSM_ESC;
        }
        else {
          buffer[index] = c;
        }
      }
      else {
        buffer[index] = c;
      }
      index += 1;
      while (!Serial.available()) {}
      c = Serial.read();
    }
    return index;
  }

  /**
   * @param buffer: the pointer to the data that will be sent.
   * @param size: the number of bytes to send.
   */
  void transmit(uint8_t *buffer, uint16_t size) {
    if (size == 0) return;

    uint8_t c;
    uint16_t index = 0;
    while (index < size) {
      c = buffer[index];
      if (c == NLSM_END) {
        Serial.write(NLSM_ESC);
        Serial.write(NLSM_ESC_END);
      }
      else if (c == NLSM_ESC) {
        Serial.write(NLSM_ESC);
        Serial.write(NLSM_ESC_ESC);
      }
      else {
        Serial.write(c);
      }
      index += 1;
    }
    Serial.write(NLSM_END);
  }
};

SerializerClass Serializer;

}  // namespace rath
