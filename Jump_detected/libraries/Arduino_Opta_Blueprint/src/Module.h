/* -------------------------------------------------------------------------- */
/* FILENAME:    Module.h
   AUTHOR:      Daniele Aimo (d.aimo@arduino.cc)
   DATE:        20230801
   REVISION:    0.0.1
   DESCRIPTION: This is the header file for the Module class
   LICENSE:     Copyright (c) 2024 Arduino SA
                This Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#ifndef OPTA_BLUE_MODULE
#define OPTA_BLUE_MODULE

#include "Arduino.h"
#include "CommonCfg.h"
#include "OptaCrc.h"
#include "Protocol.h"
#include "Wire.h"
#include <stdint.h>
/* when DETECT IN goes low it wait OPTA_CONTROLLER_DEBOUNCE_TIME *
   OPTA_CONTROLLER_DEBOUNCE_NUMBER ms before to consider the PIN really low */
#define OPTA_MODULE_DEBOUNCE_TIME_IN 1
#define OPTA_MODULE_DEBOUNCE_NUMBER_IN 50
#define OPTA_MODULE_DEBOUNCE_TIME_OUT 1
#define OPTA_MODULE_DEBOUNCE_NUMBER_OUT 100
/* time the DETECT IN is kept LOW after reset to signal other expansions
   that an expansion on the right it'exiting from reset and so a new
   assign addresses process will be take place soon */
#define OPTA_MODULE_DETECT_OUT_LOW_TIME 1000

#define WAIT_FOR_REBOOT 500

class Module {
public:
  Module();
  virtual void begin();
  virtual bool addressAcquired();
  virtual void update();
  virtual void end();
  /* must be public because is called in the RX callback */
  virtual int parse_rx();

  /* set the tx_buffer @ position pos with value v */
  void tx(uint8_t v, int pos);
  /* get value of tx_buffer @ position pos */
  uint8_t tx(int pos);
  /* get value of rx buffer @ position pos */
  uint8_t rx(int pos);
  /* set rx_buffer @ position pos with value v */
  void rx(uint8_t v, int pos);
  /* set the number of bytes received into the rx_buffer */
  void setRxNum(uint8_t n);
  /* get the number of bytes received into the rx_buffer */
  uint8_t getRxNum();
  /* set the number of bytes to be transmitted in the tx_buffer */
  void setTxNum(uint8_t n);
  /* get the number of bytes to be transmitted in the tx_buffer */
  uint8_t getTxNum();
  /* returns the pointer to the tx buffer */
  uint8_t *txPrt();

  void setRebootSent() { reboot_sent = millis(); }

  bool parse_set_address();
  bool parse_get_address_and_type();
  bool parse_reset_controller();
  bool parse_get_version();
  bool parse_reboot();
  bool parse_set_flash();
  bool parse_get_flash();

  int prepare_ans_get_address_and_type();
  int prepare_ans_get_version();
  int prepare_ans_reboot();
  int prepare_ans_get_flash();
  uint8_t getI2CAddress() { return i2c_address; }

protected:
  unsigned long int reboot_sent;
  void updatePinStatus();

  /* handle "reset": reset here means
     1. true reset from power up
     2. reset due to reset message from controller
     3. reset due digital out of a controller on the left that goes LOW */
  virtual void reset();

  volatile bool reset_required;
  volatile bool reboot_required;

  bool is_detect_in_low();

  bool is_detect_out_low();
  bool is_detect_out_high();

  void setAddress(uint8_t add);
  /* this variable need to be set in every constructor of the derived class */
  ExpansionType_t expansion_type;
  uint8_t rx_buffer[OPTA_I2C_BUFFER_DIM];
  uint8_t tx_buffer[OPTA_I2C_BUFFER_DIM];
  uint8_t address;
  uint8_t rx_num;
  uint8_t tx_num;
  uint16_t flash_add;
  uint8_t flash_dim;
  uint8_t i2c_address;
  /* to answer "faster" instead of using a single tx_buffer there will be
     multiple answer buffers (one for each request the controller can make)
     tx_buffer defined here (a few rows above will be used to handle messages
     related to assign address, this because assign address is suppose to
     be independent from the different module attached to the Controller */
  uint8_t *ans_buffer;
};

extern Module *OptaExpansion;

#endif //
