/* -------------------------------------------------------------------------- */
/* FILENAME:    OptaDigital.cpp
   AUTHOR:      Daniele Aimo (d.aimo@arduino.cc)
   DATE:        20230810
   REVISION:    0.0.1
   DESCRIPTION: Implementation of Class OptaDigital to deal with data management
                specific to Opta Digital
   LICENSE:     Copyright (c) 2024 Arduino SA
                This Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */

#if defined ARDUINO_OPTA_DIGITAL
#include "OptaDigital.h"
#include "EEPROM.h"
#include "MsgCommon.h"
#include "Protocol.h"
#include <stdint.h>

// #ifdef ARDUINO_OPTA_DIGITAL

volatile bool OptaDigital::conversion_performed = false;

/* -------------------------------------------------------------------------- */
void OptaDigital::adcCb(adc_callback_args_t *p_args) {
  /* --------------------------------------------------------------------------
   */
  if (p_args != nullptr) {
    adc_event_t ev = p_args->event;
    OptaDigital *od = (OptaDigital *)p_args->p_context;
    if (ev == ADC_EVENT_SCAN_COMPLETE) {
      OptaDigital::conversion_performed = true;
      R_ADC_ScanStop(od->getAdcCtrl());
    }
  }
}

/* -------------------------------------------------------------------------- */
/* CONSTRUCTOR - overrides the different settings specific for OPTA digital   */
/* -------------------------------------------------------------------------- */
OptaDigitalADC::OptaDigitalADC(void *ctx /* = nullptr */)
    : ADC_Container(0, nullptr, ctx) {
  cfg_extend.adc_vref_control = ADC_VREF_CONTROL_VREFH0_AVSS0;

  cfg.unit = 0;
  cfg.mode = ADC_MODE_CONTINUOUS_SCAN;

  cfg.resolution = ADC_RESOLUTION_14_BIT;
  cfg.p_callback = OptaDigital::adcCb;
  cfg.p_context = ctx;
}

/* -------------------------------------------------------------------------- */
/* CONSTRUCTOR                                                                */
/* -------------------------------------------------------------------------- */

OptaDigital::OptaDigital() : opta_adc(this) {
  in_map[0] = OPTA_DIGITAL_IN_INDEX_0;
  in_map[1] = OPTA_DIGITAL_IN_INDEX_1;
  in_map[2] = OPTA_DIGITAL_IN_INDEX_2;
  in_map[3] = OPTA_DIGITAL_IN_INDEX_3;
  in_map[4] = OPTA_DIGITAL_IN_INDEX_4;
  in_map[5] = OPTA_DIGITAL_IN_INDEX_5;
  in_map[6] = OPTA_DIGITAL_IN_INDEX_6;
  in_map[7] = OPTA_DIGITAL_IN_INDEX_7;
  in_map[8] = OPTA_DIGITAL_IN_INDEX_8;
  in_map[9] = OPTA_DIGITAL_IN_INDEX_9;
  in_map[10] = OPTA_DIGITAL_IN_INDEX_10;
  in_map[11] = OPTA_DIGITAL_IN_INDEX_11;
  in_map[12] = OPTA_DIGITAL_IN_INDEX_12;
  in_map[13] = OPTA_DIGITAL_IN_INDEX_13;
  in_map[14] = OPTA_DIGITAL_IN_INDEX_14;
  in_map[15] = OPTA_DIGITAL_IN_INDEX_15;

  out_map[0] = OPTA_DIGITAL_OUT_INDEX_0;
  out_map[1] = OPTA_DIGITAL_OUT_INDEX_1;
  out_map[2] = OPTA_DIGITAL_OUT_INDEX_2;
  out_map[3] = OPTA_DIGITAL_OUT_INDEX_3;
  out_map[4] = OPTA_DIGITAL_OUT_INDEX_4;
  out_map[5] = OPTA_DIGITAL_OUT_INDEX_5;
  out_map[6] = OPTA_DIGITAL_OUT_INDEX_6;
  out_map[7] = OPTA_DIGITAL_OUT_INDEX_7;

  for (int i = 0; i < OPTA_DIGITAL_OUT_NUM; i++) {
    digital_out[i] = false;
  }
}

/* -------------------------------------------------------------------------- */
void OptaDigital::timer_callback(timer_callback_args_t *arg) {
  /* ------------------------------------------------------------------------ */
  OptaDigital *ptr = nullptr;
  if (arg != nullptr) {
    ptr = (OptaDigital *)arg->p_context;
  }

  if (ptr != nullptr) {
    ptr->_incrementTimerCallNum();

    if (ptr->_timerElapsed()) {
      ptr->_resetTimerCallNum();
      if (ptr->_timeIsNotForever()) {
        ptr->_resetOutputs();
      }
    }
  }
}

/* -------------------------------------------------------------------------- */
void OptaDigital::set_up_timer() {
  /* ------------------------------------------------------------------------ */
  uint8_t type;
  int8_t num = FspTimer::get_available_timer(type);
  if (num >= 0) {
    timer.begin(TIMER_MODE_PERIODIC, type, num, 1000, 50, timer_callback, this);
    timer.setup_overflow_irq();
    timer.open();
    timer.start();
  }
}

/* -------------------------------------------------------------------------- */
void OptaDigital::_resetOutputs() {
  /* --------------------------------------------------------------------------
   */

  for (int i = 0; i < OPTA_DIGITAL_OUT_NUM; i++) {

    digital_out[i] = default_output[i];

    if (digital_out[i]) {
      digitalWrite(out_map[i], HIGH);
    } else {
      digitalWrite(out_map[i], LOW);
    }
  }
}

/* -------------------------------------------------------------------------- */
void OptaDigital::reset() {
  /* --------------------------------------------------------------------------
   */
  /* reset output must be called first otherwise we get some delay from
     module reset and that make the output reset uneffective */
  _resetOutputs();
  Module::reset();
}

/* -------------------------------------------------------------------------- */
/* begin                                                                      */
/* -------------------------------------------------------------------------- */
void OptaDigital::begin() {
  Module::begin();

  uint8_t digital_kind = 255;
  EEPROM.get(EXPANSION_TYPE_ADDITIONA_DATA, digital_kind);
  if (digital_kind == FLASH_OD_TYPE_MECHANICAL) {
    expansion_type = EXPANSION_OPTA_DIGITAL_MEC;
  } else if (digital_kind == FLASH_OD_TYPE_STATE_SOLID) {
    expansion_type = EXPANSION_OPTA_DIGITAL_STS;
  } else {
    expansion_type = EXPANSION_DIGITAL_INVALID;
  }
  /* initialize digital output */
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);

  digitalWrite(D0, LOW);
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);
  digitalWrite(D7, LOW);

  set_up_timer();

#if defined DEBUG_SERIAL && defined DEBUG_ADC_SETUP_OPTA_DIGITAL
  Serial.print("- Add ADC pin ");
#endif

  for (int i = OPTA_DIGITAL_FIRST_ANALOG_IN; i < OPTA_DIGITAL_LAST_ANALOG_IN;
       i++) {
#if defined DEBUG_SERIAL && defined DEBUG_ADC_SETUP_OPTA_DIGITAL
    Serial.print("- Variant PIN number: ");
    Serial.print(i);
#endif
    auto cfg_adc = getPinCfgs(i, PIN_CFG_REQ_ADC);
    if (cfg_adc[0] > 0) {
      pinPeripheral(digitalPinToBspPin(i), (uint32_t)IOPORT_CFG_ANALOG_ENABLE);
      opta_adc.channel_cfg.scan_mask |= (1 << GET_CHANNEL(cfg_adc[0]));
#if defined DEBUG_SERIAL && defined DEBUG_ADC_SETUP_OPTA_DIGITAL
      Serial.println(" OK!");
#endif
    } else {
#if defined DEBUG_SERIAL && defined DEBUG_ADC_SETUP_OPTA_DIGITAL
      Serial.println(" ERROR: NOT ADC PIN");
#endif
    }
  }

  if (IRQManager::getInstance().addADCScanEnd(&opta_adc)) {
    opta_adc.cfg.scan_end_ipl = 14;
  }

  fsp_err_t err = R_ADC_Open(&(opta_adc.ctrl), &(opta_adc.cfg));
#if defined DEBUG_SERIAL && defined DEBUG_ADC_SETUP_OPTA_DIGITAL
  Serial.print("ADC open ");
  Serial.println(err);
#endif

  err = R_ADC_ScanCfg(&(opta_adc.ctrl), &(opta_adc.channel_cfg));

#if defined DEBUG_SERIAL && defined DEBUG_ADC_SETUP_OPTA_DIGITAL
  Serial.print("ADC scan ");
  Serial.println(err);
#endif

  err = R_ADC_ScanStart(&(opta_adc.ctrl));

#if defined DEBUG_SERIAL && defined DEBUG_ADC_SETUP_OPTA_DIGITAL
  Serial.print("ADC Scan Start ");
  Serial.println(err);
#endif
}

/* -------------------------------------------------------------------------- */
bool OptaDigital::parse_set_digital() {
  /* ------------------------------------------------------------------------ */
  if (checkSetMsgReceived(rx_buffer, ARG_OPTDIG_DIGITAL_OUT,
                          LEN_OPTDIG_DIGITAL_OUT,
                          MSG_SET_OPTDIG_DIGITAL_OUT_LEN)) {
    return true;
  }
  return false;
}

/* -------------------------------------------------------------------------- */
bool OptaDigital::parse_get_digital() {
  /* ------------------------------------------------------------------------ */
  if (checkGetMsgReceived(rx_buffer, ARG_OPDIG_DIGITAL, LEN_OPDIG_DIGITAL,
                          MSG_GET_OPTDIG_LEN)) {
    return true;
  }
  return false;
}

/* -------------------------------------------------------------------------- */
bool OptaDigital::parse_get_analog() {
  /* ------------------------------------------------------------------------ */
  if (checkGetMsgReceived(rx_buffer, ARG_OPTDIG_ANALOG, LEN_OPTDIG_ANALOG,
                          MSG_GET_OPTDIG_ANALOG_LEN)) {
    return true;
  }
  return false;
}

/* ------------------------------------------------------------------------ */
bool OptaDigital::parse_get_all_analog() {
  /* ------------------------------------------------------------------------ */
  if (checkGetMsgReceived(rx_buffer, ARG_OPTDIG_ALL_ANALOG_IN,
                          LEN_OPTDIG_ALL_ANALOG_IN,
                          MSG_GET_OPTDIG_ALL_ANALOG_IN_LEN)) {
    return true;
  }
  return false;
}

/* ------------------------------------------------------------------------ */
bool OptaDigital::parse_default_and_timeout() {
  /* ---------------------------------------------------------------------- */
  if (checkSetMsgReceived(rx_buffer, ARG_OPTDIG_DEFAULT_OUT_AND_TIMEOUT,
                          LEN_OPTDIG_DEFAULT_OUT_AND_TIMEOUT,
                          MSG_SET_OPTDIG_DEFAULT_OUT_AND_TIMEOUT_LEN)) {
    return true;
  }
  return false;
}

/* ------------------------------------------------------------------------ */
int OptaDigital::prepare_ans_get_digital() {
  /* ---------------------------------------------------------------------- */
  ans_buffer = tx_buffer;

  for (int i = 0; i < ANS_MSG_OPDIG_DIGITAL_LEN; i++) {
    tx_buffer[i] = ans_get_din_buffer[i];
  }

#ifdef BP_USE_CRC
  tx_buffer[ANS_MSG_OPDIG_DIGITAL_LEN] =
      OptaCrc8::calc(tx_buffer, ANS_MSG_OPDIG_DIGITAL_LEN, 0);
  return ANS_MSG_OPDIG_DIGITAL_LEN_CRC;
#else
  return ANS_MSG_OPDIG_DIGITAL_LEN;
#endif
}

/* ------------------------------------------------------------------------- */
int OptaDigital::prepare_ans_get_analog(int index) {
  /* ----------------------------------------------------------------------- */
  int rv = 0;
  if (index > OPTA_DIGITAL_IN_NUM) {
    return rv;
  }
  ans_buffer = tx_buffer;
  tx_buffer[BP_PAYLOAD_START_POS] =
      ans_get_all_ain_buffer[BP_HEADER_DIM + 2 * index];
  tx_buffer[BP_PAYLOAD_START_POS + 1] =
      ans_get_all_ain_buffer[BP_HEADER_DIM + 2 * index + 1];

  return prepareGetAns(tx_buffer, ANS_ARG_OPTDIG_ANALOG, ANS_LEN_OPTDIG_ANALOG,
                       ANS_MSG_OPTDIG_ANALOG_LEN);
}

/* ------------------------------------------------------------------------ */
int OptaDigital::prepare_ans_default_and_timeout() {
  /* ---------------------------------------------------------------------- */
  ans_buffer = tx_buffer;
  return prepareSetAns(tx_buffer, ANS_ARG_OPTDIG_DEFAULT_OUT_AND_TIMEOUT,
                       ANS_LEN_OPTDIG_DEFAULT_OUT_AND_TIMEOUT,
                       ANS_MSG_OPTDIG_DEFAULT_OUT_AND_TIMEOUT_LEN);
}

/* ------------------------------------------------------------------------ */
int OptaDigital::prepare_ans_get_all_analog() {
  /* ---------------------------------------------------------------------- */
  ans_buffer = tx_buffer;

  for (int i = 0; i < ANS_MSG_OPTDIG_ALL_ANALOG_IN_LEN; i++) {
    tx_buffer[i] = ans_get_all_ain_buffer[i];
  }

#ifdef BP_USE_CRC
  tx_buffer[ANS_MSG_OPTDIG_ALL_ANALOG_IN_LEN] =
      OptaCrc8::calc(tx_buffer, ANS_MSG_OPTDIG_ALL_ANALOG_IN_LEN, 0);
  return ANS_MSG_OPTDIG_ALL_ANALOG_IN_LEN_CRC;
#else
  return ANS_MSG_OPTDIG_ALL_ANALOG_IN_LEN;
#endif
}

/* ------------------------------------------------------------------------ */
int OptaDigital::prepare_ans_set_digital() {
  /* ---------------------------------------------------------------------- */
  ans_buffer = tx_buffer;
  return prepareSetAns(tx_buffer, ANS_ARG_OPTDIG_DIGITAL_OUT,
                       ANS_LEN_OPTDIG_DIGITAL_OUT,
                       ANS_MSG_OPTDIG_DIGITAL_OUT_LEN);
}

/* --------------------------------------------------------------------------
 */
/* parse message specific for OPTA digital */
/* --------------------------------------------------------------------------
 */
int OptaDigital::parse_rx() {
  timer_call_num = 0;
  /* call base version because here are handled assign addresses messages
     NOTE: this must be done for every other expansion type derived from
           Module */
  int rv = Module::parse_rx();

  if (rv != -1) {
    /* no need for other msg parsing (message parsed in base version) */
    return rv;
  }

  rv = -1;
  /* set digital output */
  if (parse_set_digital()) {
    uint8_t value = rx_buffer[BP_PAYLOAD_START_POS];

    for (int i = 0; i < OPTA_DIGITAL_OUT_NUM; i++) {
      if (value & (1 << i)) {
        digital_out[i] = true;
        digitalWrite(out_map[i], HIGH);
      } else {
        digital_out[i] = false;
        digitalWrite(out_map[i], LOW);
      }
    }
    rv = prepare_ans_set_digital();
  }
  /* get digital input */
  else if (parse_get_digital()) {
    rv = prepare_ans_get_digital();
  }
/* get all analog input */
#ifdef OPTA_DIGITAL_ALLOW_ANALOG_USE
  else if (parse_get_all_analog()) {
    rv = prepare_ans_get_all_analog();
  } else if (parse_get_analog()) {
    int index = rx_buffer[BP_PAYLOAD_START_POS];
    if (index >= OPTA_DIGITAL_IN_NUM) {
      index = 0;
    }
    rv = prepare_ans_get_analog(index);
  } else if (parse_default_and_timeout()) {

    uint8_t value = rx_buffer[BP_PAYLOAD_START_POS];

    for (int i = 0; i < OPTA_DIGITAL_OUT_NUM; i++) {
      if (value & (1 << i)) {
        default_output[i] = true;
      } else {
        default_output[i] = false;
      }
    }

    timer_elapsed_ms = rx_buffer[BP_PAYLOAD_START_POS + 1];
    timer_elapsed_ms += ((uint16_t)rx_buffer[BP_PAYLOAD_START_POS + 2] << 8);

    rv = prepare_ans_default_and_timeout();
  }
#endif
  return rv;
}

#ifdef DEBUG_UPDATE_FW
/* used to recognize sw version with led blinking during fw update tests*/
void OptaDigital::debug_with_leds() {
  static unsigned long start = millis();
  static bool status = true;

  if (millis() - start > 2000) {
    start = millis();

    digital_out[0] = status;
    if (status) {
      status = false;
    } else {
      status = true;
    }
  }
}
#endif
/* --------------------------------------------------------------------------
 */
/* update */
/* --------------------------------------------------------------------------
 */
void OptaDigital::update() {
  uint16_t digital_in = 0;

#ifdef DEBUG_UPDATE_FW
  debug_with_leds();
  _updateDigitalOut();
#endif
  /* call base version because here are the update related to assign address
     NOTE: this must be done for every other expansion type derived from
           Module */
  Module::update();

  // if (Module::addressAcquired()) {
  /* always "refresh" the header of the answers */
  ans_get_din_buffer[BP_CMD_POS] = BP_ANS_GET;
  ans_get_din_buffer[BP_ARG_POS] = ANS_ARG_OPDIG_DIGITAL;
  ans_get_din_buffer[BP_LEN_POS] = ANS_LEN_OPDIG_DIGITAL;

  ans_get_all_ain_buffer[BP_CMD_POS] = BP_ANS_GET;
  ans_get_all_ain_buffer[BP_ARG_POS] = ANS_ARG_OPTDIG_ALL_ANALOG_IN;
  ans_get_all_ain_buffer[BP_LEN_POS] = ANS_LEN_OPTDIG_ALL_ANALOG_IN;

  /* update output */
  _updateDigitalOut();
  /* update input*/
  if (OptaDigital::conversion_performed) {
    OptaDigital::conversion_performed = false;

    for (int i = 0; i < MAX_ADC_CHANNELS; i++) {
      if (opta_adc.channel_cfg.scan_mask & (1 << i)) {
        // is the channel active -> yes, read it
        /* fsp_err_t err = */
        R_ADC_Read(&(opta_adc.ctrl), (adc_channel_t)i,
                   channel_analog_values + i);

#if defined DEBUG_SERIAL && defined DEBUG_SERIAL_ADC &&                        \
    defined DEBUG_SERIAL_ADC_REG
        Serial.print("[Log] Analog Channel ");
        Serial.print(i);
        Serial.print(" ");
        Serial.println(channel_analog_values[i]);
#endif
      }
    }

    for (int i = 0; i < OPTA_DIGITAL_IN_NUM; i++) {
      uint16_t value = channel_analog_values[in_map[i]];

#ifdef OPTA_DIGITAL_ALLOW_ANALOG_USE
      ans_get_all_ain_buffer[BP_PAYLOAD_START_POS + 2 * i] =
          (uint8_t)(value & 0xFF);
      ans_get_all_ain_buffer[BP_PAYLOAD_START_POS + 2 * i + 1] =
          (uint8_t)((value & 0xFF00) >> 8);
#ifdef EN_DIGITAL_STANDALONE
      analog_inputs[i] = value;
#endif
#endif

      if (value > OPTA_DIGITAL_TH_FOR_PIN_LOW) {
        digital_in |= (1 << i);
      }

#if defined DEBUG_SERIAL && defined DEBUG_OPTA_DIGITAL_INPUT
      Serial.print("AN[");
      Serial.print(i);
      Serial.print("]: ");
      Serial.print(value);
      Serial.print(" -> update digital_in ");
      Serial.println(digital_in);
#endif
    }

    ans_get_din_buffer[BP_PAYLOAD_START_POS] = (uint8_t)(digital_in & 0xFF);
    ans_get_din_buffer[BP_PAYLOAD_START_POS + 1] =
        (uint8_t)((digital_in & 0xFF00) >> 8);

    R_ADC_ScanStart(&(opta_adc.ctrl));
  }
  //}
}

#ifdef EN_DIGITAL_STANDALONE
uint16_t OptaDigital::getAnalog(uint8_t p) {
  if (p < OPTA_DIGITAL_IN_NUM) {
    return analog_inputs[p];
  }
}
#endif

/* ---------------------------------------------------------------------- */
/*       end                                                              */
/* ---------------------------------------------------------------------- */
void OptaDigital::end() { R_ADC_Close(&(opta_adc.ctrl)); }

/* ---------------------------------------------------------------------- */
void OptaDigital::_setDigitalOut(int n, bool value) {
  if (n >= 0 && n < OPTA_DIGITAL_OUT_NUM) {
    digital_out[n] = value;
  }
}

bool OptaDigital::_getDigitalOut(int n) {
  if (n >= 0 && n < OPTA_DIGITAL_OUT_NUM) {
    return digital_out[n];
  }
  return false;
}
/* ----------------------------------------------------------------------- */
void OptaDigital::_updateDigitalOut() {
  for (int i = 0; i < OPTA_DIGITAL_OUT_NUM; i++) {
    if (digital_out[i]) {
      digitalWrite(out_map[i], HIGH);
    } else {
      digitalWrite(out_map[i], LOW);
    }
  }
}

#endif
