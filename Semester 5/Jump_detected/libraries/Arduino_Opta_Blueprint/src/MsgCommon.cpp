/* -------------------------------------------------------------------------- */
/* FILE NAME:   MsgCommon.cpp
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240124
   DESCRIPTION: Provide common functions to check/set the header and crc of
                I2C messages
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */
#include "MsgCommon.h"
#include "Arduino.h"
#include "OptaCrc.h"
#include "Protocol.h"
#include <stdint.h>

// #define DEBUG_MESSAGE_ON_SERIAL

static bool checkSet(uint8_t *buffer, uint8_t arg, uint8_t len);
static bool checkGet(uint8_t *buffer, uint8_t arg, uint8_t len);
static bool checkAnsSet(uint8_t *buffer, uint8_t arg, uint8_t len);
static bool checkAnsGet(uint8_t *buffer, uint8_t arg, uint8_t len);
static bool checkCrc(uint8_t *buffer, uint8_t mlen);
static void prepareSet(uint8_t *buffer, uint8_t arg, uint8_t len);
static void prepareGet(uint8_t *buffer, uint8_t arg, uint8_t len);
static void prepareAnsSet(uint8_t *buffer, uint8_t arg, uint8_t len);
static void prepareAnsGet(uint8_t *buffer, uint8_t arg, uint8_t len);
static uint8_t addCrc(uint8_t *buffer, uint8_t mlen);

#ifdef DEBUG_MESSAGE_ON_SERIAL
void debug_message(const char *log, uint8_t *m, uint8_t mlen) {
  Serial.println();
  Serial.println(log);
  Serial.print("(" + String(mlen) + "): ");
  for (int i = 0; i < mlen; i++) {
    if (m[i] < 0x10) {
      Serial.print('0');
    }
    Serial.print(m[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}
#endif

bool checkSet(uint8_t *buffer, uint8_t arg, uint8_t len) {
  if (buffer[BP_CMD_POS] == BP_CMD_SET && buffer[BP_ARG_POS] == arg &&
      buffer[BP_LEN_POS] == len) {
    return true;
  }
  return false;
}
bool checkGet(uint8_t *buffer, uint8_t arg, uint8_t len) {
  if (buffer[BP_CMD_POS] == BP_CMD_GET && buffer[BP_ARG_POS] == arg &&
      buffer[BP_LEN_POS] == len) {
    return true;
  }
  return false;
}

bool checkAnsGet(uint8_t *buffer, uint8_t arg, uint8_t len) {
  if (buffer[BP_CMD_POS] == BP_ANS_GET && buffer[BP_ARG_POS] == arg &&
      buffer[BP_LEN_POS] == len) {
    return true;
  }
  return false;
}

bool checkAnsSet(uint8_t *buffer, uint8_t arg, uint8_t len) {
  if (buffer[BP_CMD_POS] == BP_ANS_SET && buffer[BP_ARG_POS] == arg &&
      buffer[BP_LEN_POS] == len) {
    return true;
  }
  return false;
}

bool checkCrc(uint8_t *buffer, uint8_t mlen) {
  bool rv = true;

#ifdef BP_USE_CRC
  if (!OptaCrc8::verify(buffer[mlen], buffer, mlen)) {
    rv = false;
  }
#endif
#ifdef DEBUG_MESSAGE_ON_SERIAL
  Serial.println("CRC is Ok!");
#endif
  return rv;
}

bool checkSetMsgReceived(uint8_t *buffer, uint8_t arg, uint8_t len,
                         uint8_t mlen) {
#ifdef DEBUG_MESSAGE_ON_SERIAL
  debug_message("-RX SET msg", buffer, mlen);
#endif
  if (checkSet(buffer, arg, len)) {
    if (checkCrc(buffer, mlen)) {
      return true;
    }
  }
  return false;
}

bool checkGetMsgReceived(uint8_t *buffer, uint8_t arg, uint8_t len,
                         uint8_t mlen) {
#ifdef DEBUG_MESSAGE_ON_SERIAL
  debug_message("-RX GET msg", buffer, mlen);
#endif
  if (checkGet(buffer, arg, len)) {
    if (checkCrc(buffer, mlen)) {
      return true;
    }
  }
  return false;
}

bool checkAnsGetReceived(uint8_t *buffer, uint8_t arg, uint8_t len,
                         uint8_t mlen) {
#ifdef DEBUG_MESSAGE_ON_SERIAL
  debug_message("-RX ANS get msg", buffer, mlen);
#endif
  if (checkAnsGet(buffer, arg, len)) {
    if (checkCrc(buffer, mlen)) {
      return true;
    }
  }
  return false;
}

bool checkAnsSetReceived(uint8_t *buffer, uint8_t arg, uint8_t len,
                         uint8_t mlen) {
#ifdef DEBUG_MESSAGE_ON_SERIAL
  debug_message("-RX ANS "
                "set msg",
                buffer, mlen);
#endif
  if (checkAnsSet(buffer, arg, len)) {
    if (checkCrc(buffer, mlen)) {
      return true;
    }
  }
  return false;
}

void prepareSet(uint8_t *buffer, uint8_t arg, uint8_t len) {
  buffer[BP_CMD_POS] = BP_CMD_SET;
  buffer[BP_ARG_POS] = arg;
  buffer[BP_LEN_POS] = len;
}

void prepareGet(uint8_t *buffer, uint8_t arg, uint8_t len) {
  buffer[BP_CMD_POS] = BP_CMD_GET;
  buffer[BP_ARG_POS] = arg;
  buffer[BP_LEN_POS] = len;
}

void prepareAnsSet(uint8_t *buffer, uint8_t arg, uint8_t len) {
  buffer[BP_CMD_POS] = BP_ANS_SET;
  buffer[BP_ARG_POS] = arg;
  buffer[BP_LEN_POS] = len;
}

void prepareAnsGet(uint8_t *buffer, uint8_t arg, uint8_t len) {
  buffer[BP_CMD_POS] = BP_ANS_GET;
  buffer[BP_ARG_POS] = arg;
  buffer[BP_LEN_POS] = len;
}

uint8_t addCrc(uint8_t *buffer, uint8_t mlen) {
#ifdef BP_USE_CRC
  buffer[mlen] = OptaCrc8::calc(buffer, mlen, 0);
  return mlen + 1;
#endif
  return mlen;
}

uint8_t prepareSetMsg(uint8_t *buffer, uint8_t arg, uint8_t len, uint8_t mlen) {
  prepareSet(buffer, arg, len);
  uint8_t rv = addCrc(buffer, mlen);

#ifdef DEBUG_MESSAGE_ON_SERIAL
  debug_message("-TX SET "
                "msg",
                buffer, mlen);
#endif
  return rv;
}

uint8_t prepareGetMsg(uint8_t *buffer, uint8_t arg, uint8_t len, uint8_t mlen) {
  prepareGet(buffer, arg, len);
  uint8_t rv = addCrc(buffer, mlen);

#ifdef DEBUG_MESSAGE_ON_SERIAL
  debug_message("-TX GET "
                "msg",
                buffer, mlen);
#endif
  return rv;
}
uint8_t prepareSetAns(uint8_t *buffer, uint8_t arg, uint8_t len, uint8_t mlen) {
  prepareAnsSet(buffer, arg, len);
  uint8_t rv = addCrc(buffer, mlen);

#ifdef DEBUG_MESSAGE_ON_SERIAL
  debug_message("-TX ANS "
                "set msg",
                buffer, mlen);
#endif
  return rv;
}
uint8_t prepareGetAns(uint8_t *buffer, uint8_t arg, uint8_t len, uint8_t mlen) {
  prepareAnsGet(buffer, arg, len);
  uint8_t rv = addCrc(buffer, mlen);

#ifdef DEBUG_MESSAGE_ON_SERIAL
  debug_message("-TX ANS "
                "get msg",
                buffer, mlen);
#endif
  return rv;
}
