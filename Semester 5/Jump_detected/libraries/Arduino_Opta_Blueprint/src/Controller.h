/* -------------------------------------------------------------------------- */
/* FILENAME:    Controller.h
   AUTHOR:      Daniele Aimo (d.aimo@arduino.cc)
   DATE:        20230801
   REVISION:    0.0.1
   DESCRIPTION: This is the header file for the OptaController class
   LICENSE:     Copyright (c) 2024 Arduino SA
                This Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#ifndef OPTA_BLUE_CONTROLLER
#define OPTA_BLUE_CONTROLLER

#include "DigitalExpansion.h"
#define SEND_RESULT_OK 0
#define SEND_RESULT_WRONG_EXPANSION_INDEX 1
#define SEND_RESULT_WRONG_EXPANSION_ATTRIBUTES 2
#define SEND_RESULT_NO_DATA_TO_TRANSMIT 3
#define SEND_RESULT_COMM_TIMEOUT 4

/* since this library can be used both for OPTA and OPTA DIGITAL then the code
   for controller (which is used only for OPTA) use this define to avoid
   compilig when OPTA DIGITAL is used*/
#include "Arduino.h"
#include "CommonCfg.h"
#include "ControllerCfg.h"
#include "Expansion.h"
#include "OptaCrc.h"
#include "Protocol.h"
#include "Wire.h"
#include "sys/_stdint.h"
#include <cstdint>
#include <sys/types.h>
#include <vector>

class Controller;

using CommErr_f = void (*)(int device, int code);

using reset_exp_f = void (*)(Controller *ptr);

using namespace Opta;

class ResetCb {
public:
  reset_exp_f fnc;
  ResetCb() : fnc(nullptr) {}
  void call(Controller *ptr) {
    if (fnc != nullptr) {
      fnc(ptr);
    }
  }
};

// namespace Opta {

class Controller {
public:
  Controller();
  ~Controller();

  /* ----------------------------------------------------------- */

  /* initialize the controller it perform the assign address process */
  void begin();
  void update();
  /* performs the actual assign address process it has to be called periodically
   * in the loop to support hot-plug expansion attachment */
  void checkForExpansions();

  /* ----------------------------------------------------------- */

  /* return the number of expansion discovered */
  uint8_t getExpansionNum();
  /* return the type of expansion */
  ExpansionType_t getExpansionType(uint8_t i);
  /* return the I2C address of the expansion */
  uint8_t getExpansionI2Caddress(uint8_t i);
  bool getFwVersion(uint8_t i, uint8_t &major, uint8_t &minor,
                    uint8_t &release);

  /* ----------------------------------------------------------- */

  /* Get the i-th expansion by copy */
  Expansion &getExpansion(int device);
  /* get the i-th expansion by pointer */
  Expansion *getExpansionPtr(int device);

  /* ----------------------------------------------------------- */

  /* send the content of the I2C tx buffer
   * send n bytes from the tx_buffer
   * wait for r bytes as answer from the device */
  uint8_t send(int add, int device, ExpansionType_t type, int n, int r);
  uint8_t *getTxBuffer() { return tx_buffer; }
  uint8_t *getRxBuffer() { return rx_buffer; }
  void resetRxBuffer();
  void setTx(uint8_t value, uint8_t pos);
  uint8_t getRx(uint8_t pos);
  int getLastTxArgument() { return tx_buffer[BP_ARG_POS]; }

  /* ----------------------------------------------------------- */

  bool rebootExpansion(uint8_t i);
  void setExpStartUpCb(reset_exp_f f);
  void setFailedCommCb(CommErr_f f);

  void updateRegs(Expansion &exp) {
    if (exp.getIndex() < OPTA_CONTROLLER_MAX_EXPANSION_NUM) {
      if (expansions[exp.getIndex()] != nullptr &&
          exp.getI2CAddress() == exp_add[exp.getIndex()] &&
          exp.getType() == exp_type[exp.getIndex()]) {
        expansions[exp.getIndex()]->updateRegs(exp);
      }
    }
  }

  /* ----------------------------------------------------------------------- */
  /* DEPRECATED functions: available for DIGITAL expansions                  */
  /* ----------------------------------------------------------------------- */

  PinStatus digitalReadOpta(uint8_t device, uint8_t pin, bool update = false);
  void digitalWriteOpta(uint8_t device, uint8_t pin, PinStatus st,
                        bool update = false);
  int analogReadOpta(uint8_t device, uint8_t pin, bool update = true);
  bool updateDigitalsOut(uint8_t device);
  void updateDigitalsIn(uint8_t device);
  void updateDigitals(uint8_t device);
  void updateAnalogsIn(uint8_t device);
  void updateAnalogs(uint8_t device);
  void beginOdDefaults(uint8_t device, bool d[OPTA_DIGITAL_OUT_NUM],
                       uint16_t timeout) {
    DigitalExpansion::setDefault(*this, device,
                                 DigitalExpansion::calcDefault(d[0], d[1], d[2],
                                                               d[3], d[4], d[5],
                                                               d[6], d[7]),
                                 timeout);
  }

  /* set at most 32 byte into ROM storage */
  void setProductionData(uint8_t device, uint8_t *data, uint8_t dlen);
  /* set opta digital mechanical */
  void setOdMechanical(uint8_t device);
  void setOdStateSolid(uint8_t device);

private:
  /* controller receiving buffer */
  uint8_t rx_buffer[OPTA_I2C_BUFFER_DIM];
  /* controller transmission buffer */
  uint8_t tx_buffer[OPTA_I2C_BUFFER_DIM];
  /* number of bytes received */
  uint8_t rx_num;

  /* used to set temporary address during assign address process*/
  uint8_t tmp_address;
  /* used to store the number of device during temporary address assignment */
  int8_t tmp_num_of_exp;
  /* used to store the temporary address of each expansion */
  uint8_t tmp_exp_add[OPTA_CONTROLLER_MAX_EXPANSION_NUM];
  /* used to store the type of expansion during the temporary address
   * assignment */
  ExpansionType_t tmp_exp_type[OPTA_CONTROLLER_MAX_EXPANSION_NUM];

  /* used to assign permanent address at the end of assign address process */
  int8_t address;
  /* number of expansion discovered */
  uint8_t num_of_exp;
  /* expansion addresses */
  uint8_t exp_add[OPTA_CONTROLLER_MAX_EXPANSION_NUM];
  /* expansion types */
  ExpansionType_t exp_type[OPTA_CONTROLLER_MAX_EXPANSION_NUM];
  /* expansions arrays */
  Expansion *expansions[OPTA_CONTROLLER_MAX_EXPANSION_NUM];
  /* vector of expansions callbacks, one for each expansion type */
  std::vector<ResetCb> reset_cbs;

  /* ---------------  generic message handling functions ----------------- */

  bool wait_for_device_answer(uint8_t device, uint8_t wait_for);

  /* ---------------- message preparation functions ---------------------- */

  uint8_t msg_set_address(uint8_t add);
  uint8_t msg_get_address_and_type();
  uint8_t msg_opta_reset();
  uint8_t msg_opta_reboot();

  /* -------------------- parse message functions ------------------------ */

  bool parse_address_and_type(int slave_address);
  bool parse_opta_reboot();

  void _send(int add, int n, int r);

  bool is_detect_high();
  bool is_detect_low();

  CommErr_f failed_i2c_comm;
};

extern Controller OptaController;

//} // namespace Opta

#endif
