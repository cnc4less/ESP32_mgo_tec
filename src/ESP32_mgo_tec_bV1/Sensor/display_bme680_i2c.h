/*
  display_bme680_i2c.h - for Arduino core for the ESP32.
  Beta version 1.0.0
  
The MIT License (MIT)

Copyright (c) 2018 Mgo-tec. All rights reserved.
Blog URL ---> https://www.mgo-tec.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Use BOSCH BME680 Driver.
Copyright (C) 2016 - 2017 Bosch Sensortec GmbH
The 3-Clause BSD License
URL: https://github.com/BoschSensortec/BME680_driver

Use Arduino Time Library ( TimeLib.h )
time.c - low level time and date functions
Copyright (c) Michael Margolis 2009-2014
LGPL ver2.1
https://github.com/PaulStoffregen/Time
*/

#ifndef MGO_TEC_ESP32_DISPLAY_BME680_I2C_H_INCLUDED_
#define MGO_TEC_ESP32_DISPLAY_BME680_I2C_H_INCLUDED_

#include <Arduino.h>
#include <Wire.h>
#include <TimeLib.h> //Use Arduino time library ver1.5-
#include "ESP32_mgo_tec_bV1/LCD_driver/ili9341_spi.h"
#include "ESP32_mgo_tec_bV1/SD_font_read/shinonome.h"

extern mgo_tec_esp32_bv1::ILI9341Spi LCD;
extern mgo_tec_esp32_bv1::SdShinonomeFont SFR;

// In the display_bme680_i2c.h file
namespace mgo_tec_esp32_bv1 {

// All declarations are within the namespace scope.
// Notice the lack of indentation.

class DisplayBme680I2c
{
private:
  uint32_t mp_get_data_last_time = 0;
  uint32_t mp_plot_graph_last_time = 0;
  uint32_t mp_disp_min_max_last_time = 0;
  String mp_prev_pres_str, mp_prev_temp_str, mp_prev_hum_str, mp_prev_gas_str;
  boolean isTime_plot_heap_create = false;

public:
  enum DispState { NoWrite = -1, WriteDataError = 0, WriteGoodData = 1 }
    DispStatus = NoWrite;

  enum TimeMsrMod { Period3sec = 0, Period6sec, Period12sec, Period36sec, Period1m12s, Period4m48s }
    TimeMeasureMode = Period3sec;
  
  float m_fTemperature = -500.0;
  float m_fPressure = -500.0;
  float m_fHumidity = -500.0;
  float m_fGas = 0.0;
  int16_t m_iTemperature = -500;
  int16_t m_iPressure = -500;
  int16_t m_iHumidity = -500;
  uint32_t m_iGas = 0;

  String m_pres_str, m_temp_str, m_hum_str, m_gas_str;

  uint32_t m_graph_plot_period = 3000;
  uint16_t m_meas_period = 3000; //温度・湿度の精度が良いのは3000ms程度
  uint16_t m_auto_meas_period = 3000; //16bit値。ガスセンサが最低限計測可能になる時間間隔が自動計算される値。
  int8_t m_time_measure_mode = 0;
  boolean m_graph_disp_ok = false;
  boolean m_value_disp_ok = false;
  boolean m_gas_value_disp_ok = false;
  boolean m_isNewData = false;
  boolean m_isNewGraphPlotData = false;
  boolean m_isDisp_min_max = false;
  boolean m_isShowing_min_max = false;

  uint32_t *m_array_time_plot;

private:
  boolean createNewArray( GraphParameter &graph );
  void deleteArray( GraphParameter &graph );
  boolean createNewTimePlotArray();
  void deleteTimePlotArray();
  void userDelayMs( uint32_t period );

public:
  void initBme680Force( int sda, int scl, uint32_t freq, int8_t sensor_ambient_temp );
  void getData();
  DispState getDataInteger( float *fT, float *fP, float *fH, float *gas_data );
  void serialPrintSensorData();
  void initGraphAll( GraphParameter &pres_graph, GraphParameter &temp_graph, GraphParameter &hum_graph, GraphParameter &gas_graph, FontParameter &graph_font );
  void reDrawGraphFrame( GraphParameter &pres_graph, GraphParameter &temp_graph, GraphParameter &hum_graph, GraphParameter &gas_graph, FontParameter &graph_font );
  void reDrawGraphAll( GraphParameter &pres_graph, GraphParameter &temp_graph, GraphParameter &hum_graph, GraphParameter &gas_graph, FontParameter &graph_font );
  void drawGraphLineAll( GraphParameter &pres_graph, GraphParameter &temp_graph, GraphParameter &hum_graph, GraphParameter &gas_graph );
  void drawGraphNowAll( GraphParameter &pres_graph, GraphParameter &temp_graph, GraphParameter &hum_graph, GraphParameter &gas_graph );
  void initDispValue( FontParameter &pres_font, FontParameter &temp_font, FontParameter &hum_font, int16_t x0, int16_t y0, char unit_c[][5] );
  void initDispGasValue( FontParameter &gas_font, int16_t x0, int16_t y0, char unit_c[5] );
  void displayValue( FontParameter &pres_font, FontParameter &temp_font, FontParameter &hum_font );
  void displayGasValue( FontParameter &gas_font );
  void displayMinMaxValue( GraphParameter &pres_graph, GraphParameter &temp_graph, GraphParameter &hum_graph, GraphParameter &gas_graph, FontParameter &pres_font, FontParameter &temp_font, FontParameter &hum_font, FontParameter &gas_font );
  int32_t calcGasAverageValue( GraphParameter &gas_graph, uint16_t sample );
};

}// namespace mgo_tec_esp32_bv1

#endif
