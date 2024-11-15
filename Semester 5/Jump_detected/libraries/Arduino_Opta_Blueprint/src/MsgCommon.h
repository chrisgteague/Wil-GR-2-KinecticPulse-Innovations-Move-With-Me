/* -------------------------------------------------------------------------- */
/* FILE NAME:   MsgCommon.h
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240124
   DESCRIPTION:
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#ifndef MSGCOMMON_INCLUDED
#define MSGCOMMON_INCLUDED

#include <cstdint>
#include <stdint.h>
bool checkSetMsgReceived(uint8_t *buffer, uint8_t arg, uint8_t len,
                         uint8_t mlen);

bool checkGetMsgReceived(uint8_t *buffer, uint8_t arg, uint8_t len,
                         uint8_t mlen);

bool checkAnsGetReceived(uint8_t *buffer, uint8_t arg, uint8_t len,
                         uint8_t mlen);

bool checkAnsSetReceived(uint8_t *buffer, uint8_t arg, uint8_t len,
                         uint8_t mlen);

uint8_t prepareSetMsg(uint8_t *buffer, uint8_t arg, uint8_t len, uint8_t mlen);
uint8_t prepareGetMsg(uint8_t *buffer, uint8_t arg, uint8_t len, uint8_t mlen);
uint8_t prepareSetAns(uint8_t *buffer, uint8_t arg, uint8_t len, uint8_t mlen);
uint8_t prepareGetAns(uint8_t *buffer, uint8_t arg, uint8_t len, uint8_t mlen);

#endif
