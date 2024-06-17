/* -------------------------------------------------------------------------- */
/* FILE NAME:   DigitalMechExpansion.cpp
   AUTHOR:      Daniele Aimo
   EMAIL:       d.aimo@arduino.cc
   DATE:        20240209
   DESCRIPTION:
   LICENSE:     Copyright (c) 2024 Arduino SA
                his Source Code Form is subject to the terms fo the Mozilla
                Public License (MPL), v 2.0. You can obtain a copy of the MPL
                at http://mozilla.org/MPL/2.0/.
   NOTES:                                                                     */
/* -------------------------------------------------------------------------- */
#include "DigitalMechExpansion.h"
#include "Controller.h"
#ifdef ARDUINO_OPTA
namespace Opta {
DigitalMechExpansion::DigitalMechExpansion() {}

DigitalMechExpansion::~DigitalMechExpansion() {}

DigitalMechExpansion::DigitalMechExpansion(Expansion &other) {
  DigitalMechExpansion &dme = (DigitalMechExpansion &)other;
  if (other.getType() == EXPANSION_OPTA_DIGITAL_MEC) {
    iregs = dme.iregs;
    fregs = dme.fregs;
    type = other.getType();
    i2c_address = other.getI2CAddress();
    ctrl = other.getCtrl();
    index = other.getIndex();
    if (ctrl != nullptr) {
      ctrl->setExpStartUpCb(DigitalExpansion::startUp);
    }
  } else {
    type = EXPANSION_NOT_VALID;
    i2c_address = 0;
    ctrl = other.getCtrl();
    if (ctrl != nullptr) {
      ctrl->setExpStartUpCb(DigitalExpansion::startUp);
    }
    index = 255;
  }
}

} // namespace Opta
#endif
