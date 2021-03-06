/*
  mgo_tec_bv1_m5stack_sd_init1.h
  Beta version 1.0.1
*/

#ifndef MGO_TEC_BV1_M5STACK_SD_INIT1_H_INCLUDED_
#define MGO_TEC_BV1_M5STACK_SD_INIT1_H_INCLUDED_

#include "mgo_tec_bv1_m5stack_sd_init.h"
#include "ESP32_mgo_tec_bV1/Display/ILI9341_SD/watch.h"
#include "ESP32_mgo_tec_bV1/Display/ILI9341_SD/display_shinonome_fnt.h"
#include "ESP32_mgo_tec_bV1/Display/ILI9341_SD/message_window.h"

// In the mgo_tec_bv1_m5stack_sd_init1.h file
namespace mgo_tec_esp32_bv1 {

// All declarations are within the namespace scope.
// Notice the lack of indentation.

class MgotecM5stackSdInit1 : public MgotecM5stackSdInit
{
public:
  FontParameter font[8];
  ScrolleParameter scl_set[8];
  DispShinonomeFnt disp_fnt[8];
};

}// namespace mgo_tec_esp32_bv1

#endif
