/*
  watch.cpp - for Arduino core for the ESP32.
  ( Use LCD ILI9341 and SD )
  Beta version 1.0.4
  
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

Use Arduino Time Library ( TimeLib.h )
time.c - low level time and date functions
Copyright (c) Michael Margolis 2009-2014
LGPL ver2.1
https://github.com/PaulStoffregen/Time
*/

#include "ESP32_mgo_tec_bV1/Display/ILI9341_SD/watch.h"

// In the watch.cpp file
namespace mgo_tec_esp32_bv1 {

// Definition of functions is within scope of the namespace.

//*******************************************************
void ILI9341Watch::init( uint16_t x0, uint16_t y0, uint8_t Xsize, uint8_t Ysize ){
  uint8_t num;
  for( num = 0; num < 4; num++ ){ //時、分のフォント色設定
    LCD.scrolleFontColorSet( font[num] );
  }
  for( num = 4; num < 6; num++ ){ //秒のフォント色設定
    LCD.scrolleFontColorSet( font[num] );
  }
  ILI9341Watch::initDefNum();
  ILI9341Watch::watchFontSetup( x0, y0, Xsize, Ysize ); //時計フォントセット
}
//*******************************************************
void ILI9341Watch::init2( uint16_t x0, uint16_t y0, uint8_t Xsize, uint8_t Ysize ){
  uint8_t num;
  for( num = 0; num < 4; num++ ){ //時、分のフォント色設定
    LCD.scrolleFontColorSet( font[num] );
  }
  for( num = 4; num < 6; num++ ){ //秒のフォント色設定
    LCD.scrolleFontColorSet( font[num] );
  }
  ILI9341Watch::initDefNum();
  ILI9341Watch::watchFontSetup2( x0, y0, Xsize, Ysize ); //時計フォントセット
}
//*******************************************************
void ILI9341Watch::initNormal( uint16_t x0, uint16_t y0, uint8_t Xsize, uint8_t Ysize ){
  ILI9341Watch::initDefNum();
  ILI9341Watch::setupNormalFont( x0, y0, Xsize, Ysize ); //時計フォントセット
  ILI9341Watch::watchDispReset();
}
//*******************************************************
void ILI9341Watch::initDefNum(){
  uint8_t num = 0; //時、十の位の文字列をフォント変換
  ILI9341Watch::watchStrFontConv( num, "　１２３４５６７８９" );
  for( num = 1; num < 6; num++ ){
    ILI9341Watch::watchStrFontConv( num, "０１２３４５６７８９" );
  }
}
//*******************************************************
void ILI9341Watch::setupNormalFont( uint16_t x0, uint16_t y0, uint8_t Xsize, uint8_t Ysize ){
  uint8_t num = 0;
  for( num = 0; num < 4; num++ ){
    font[num].Xsize = Xsize, font[num].Ysize = Ysize;
    font[num].y0 = y0; //各文字開始位置
  }
  num = 0; //時表示十の位
  font[num].x0 = 0; //各文字開始位置

  num = 1; //時表示一の位
  font[num].x0 = 48;

  num = 2; //分表示十の位
  font[num].x0 = 120;

  num = 3; //分表示一の位
  font[num].x0 = 168;

  num = 4; //秒表示十の位
  if( Xsize != 1 ) font[num].Xsize = Xsize - 1;
  if( Ysize != 1 ) font[num].Ysize = Ysize - 1;
  font[num].x0 = 240;
  font[num].y0 = y0 + 16;
  if( Ysize == 1 ) font[num].y0 = y0;

  num = 5; //秒表示一の位
  if( Xsize != 1 ) font[num].Xsize = Xsize - 1;
  if( Ysize != 1 ) font[num].Ysize = Ysize - 1;
  font[num].x0 = 272;
  font[num].y0 = y0 + 16;
  if( Ysize == 1 ) font[num].y0 = y0;

  //コロンフォント初期化
  colon1_font.x0 = 96, colon1_font.y0 = y0;
  colon1_font.Xsize = Xsize, colon1_font.Ysize = Ysize;
  colon2_font.x0 = 216, colon2_font.y0 = y0;
  colon2_font.Xsize = Xsize, colon2_font.Ysize = Ysize;
}
//*******************************************************
void ILI9341Watch::watchFontSetup( uint16_t x0, uint16_t y0, uint8_t Xsize, uint8_t Ysize ){
  uint8_t num = 0;
  uint8_t X_max_txt = 2; //横スクロール時刻文字、各桁全角１文字表示(半角２文字分）
  uint8_t Y_max_txt = 1; //縦スクロール時刻文字、各桁全角１文字表示(半角２文字分）
  for( num = 0; num < MAX_TXT_NUM; num++ ){
    LCD.scrolleArrayDelete( scl_set[num] ); //ヒープ領域メモリ、配列消去
  }
  for( num = 0; num < 4; num++ ){
    font[num].Xsize = Xsize, font[num].Ysize = Ysize;
    font[num].y0 = y0; //各文字開始位置
  }

  num = 0; //時表示十の位
  scl_set[num].interval = 50;
  font[num].x0 = 0; //各文字開始位置
  //水平方向スクロール文字フォント配列生成、および初期化
  LCD.XscrolleFontArrayInit( font[num], scl_set[num], X_max_txt, font[num].Xsize, font[num].Ysize);

  num = 1; //時表示一の位
  scl_set[num].interval = 50;
  font[num].x0 = 48;
  //垂直方向スクロール文字フォント配列生成、および初期化
  LCD.YscrolleFontArrayInit( font[num], scl_set[num], Y_max_txt, font[num].Xsize, font[num].Ysize );

  num = 2; //分表示十の位
  scl_set[num].interval = 50;
  font[num].x0 = 120;
  //水平方向スクロール文字フォント配列生成、および初期化
  LCD.XscrolleFontArrayInit( font[num], scl_set[num], X_max_txt, font[num].Xsize, font[num].Ysize );

  num = 3; //分表示一の位
  scl_set[num].interval = 50;
  font[num].x0 = 168;
  //垂直方向スクロール文字フォント配列生成、および初期化
  LCD.YscrolleFontArrayInit( font[num], scl_set[num], Y_max_txt, font[num].Xsize, font[num].Ysize );  LCD.scrolleFontSetUp( font[num], scl_set[num] );

  num = 4; //秒表示十の位
  scl_set[num].interval = 30;
  if( Xsize != 1 ) font[num].Xsize = Xsize - 1;
  if( Ysize != 1 ) font[num].Ysize = Ysize - 1;
  font[num].x0 = 240;
  font[num].y0 = y0 + 16;
  if( Ysize == 1 ) font[num].y0 = y0;
  //水平方向スクロール文字フォント配列生成、および初期化
  LCD.XscrolleFontArrayInit( font[num], scl_set[num], X_max_txt, font[num].Xsize, font[num].Ysize );

  num = 5; //秒表示一の位
  scl_set[num].interval = 24;
  if( Xsize != 1 ) font[num].Xsize = Xsize - 1;
  if( Ysize != 1 ) font[num].Ysize = Ysize - 1;
  font[num].x0 = 272;
  font[num].y0 = y0 + 16;
  if( Ysize == 1 ) font[num].y0 = y0;
  //垂直方向スクロール文字フォント配列生成、および初期化
  LCD.YscrolleFontArrayInit( font[num], scl_set[num], Y_max_txt, font[num].Xsize, font[num].Ysize );

  for( num = 0; num < MAX_TXT_NUM; num++ ){
    LCD.scrolleFontSetUp( font[num], scl_set[num] );
  }

  //コロンフォント初期化
  colon1_font.x0 = 96, colon1_font.y0 = y0;
  colon1_font.Xsize = Xsize, colon1_font.Ysize = Ysize;

  colon2_font.x0 = 216, colon2_font.y0 = y0;
  colon2_font.Xsize = Xsize, colon2_font.Ysize = Ysize;

  watchDispReset();

  log_v("Free Heap Size = %d", esp_get_free_heap_size());
}
  //*******************************************************
void ILI9341Watch::watchFontSetup2( uint16_t x0, uint16_t y0, uint8_t Xsize, uint8_t Ysize ){
  uint8_t num = 0;
  uint8_t X_max_txt = 2; //横スクロール時刻文字、各桁全角１文字表示(半角２文字分）
  uint8_t Y_max_txt = 1; //縦スクロール時刻文字、各桁全角１文字表示(半角２文字分）
  for( num = 0; num < MAX_TXT_NUM; num++ ){
    LCD.scrolleArrayDelete( scl_set[num] ); //ヒープ領域メモリ、配列消去
  }
  for( num = 0; num < 6; num++ ){
    font[num].Xsize = Xsize, font[num].Ysize = Ysize;
    font[num].y0 = y0; //各文字開始位置
  }

  num = 0; //時表示十の位
  scl_set[num].interval = 50;
  font[num].x0 = 0; //各文字開始位置
  //水平方向スクロール文字フォント配列生成、および初期化
  LCD.XscrolleFontArrayInit( font[num], scl_set[num], X_max_txt, font[num].Xsize, font[num].Ysize);

  num = 1; //時表示一の位
  scl_set[num].interval = 50;
  font[num].x0 = 48;
  //垂直方向スクロール文字フォント配列生成、および初期化
  LCD.YscrolleFontArrayInit( font[num], scl_set[num], Y_max_txt, font[num].Xsize, font[num].Ysize );

  num = 2; //分表示十の位
  scl_set[num].interval = 50;
  font[num].x0 = 120;
  //水平方向スクロール文字フォント配列生成、および初期化
  LCD.XscrolleFontArrayInit( font[num], scl_set[num], X_max_txt, font[num].Xsize, font[num].Ysize );

  num = 3; //分表示一の位
  scl_set[num].interval = 50;
  font[num].x0 = 168;
  //垂直方向スクロール文字フォント配列生成、および初期化
  LCD.YscrolleFontArrayInit( font[num], scl_set[num], Y_max_txt, font[num].Xsize, font[num].Ysize );  LCD.scrolleFontSetUp( font[num], scl_set[num] );

  num = 4; //秒表示十の位
  scl_set[num].interval = 30;
//  if( Xsize != 1 ) font[num].Xsize = Xsize - 1;
//  if( Ysize != 1 ) font[num].Ysize = Ysize - 1;
  font[num].x0 = 240;
//  font[num].y0 = y0 + 16;
//  if( Ysize == 1 ) font[num].y0 = y0;
  //水平方向スクロール文字フォント配列生成、および初期化
  LCD.XscrolleFontArrayInit( font[num], scl_set[num], X_max_txt, font[num].Xsize, font[num].Ysize );

  num = 5; //秒表示一の位
  scl_set[num].interval = 24;
//  if( Xsize != 1 ) font[num].Xsize = Xsize - 1;
//  if( Ysize != 1 ) font[num].Ysize = Ysize - 1;
  font[num].x0 = 272;
//  font[num].y0 = y0 + 16;
//  if( Ysize == 1 ) font[num].y0 = y0;
  //垂直方向スクロール文字フォント配列生成、および初期化
  LCD.YscrolleFontArrayInit( font[num], scl_set[num], Y_max_txt, font[num].Xsize, font[num].Ysize );

  for( num = 0; num < MAX_TXT_NUM; num++ ){
    LCD.scrolleFontSetUp( font[num], scl_set[num] );
  }

  //コロンフォント初期化
  colon1_font.x0 = 96, colon1_font.y0 = y0;
  colon1_font.Xsize = Xsize, colon1_font.Ysize = Ysize;

  colon2_font.x0 = 216, colon2_font.y0 = y0;
  colon2_font.Xsize = Xsize, colon2_font.Ysize = Ysize;

  watchDispReset();

  log_v("Free Heap Size = %d", esp_get_free_heap_size());
}
//****** LCD ILI9341 ディスプレイ初期化 ***********
void ILI9341Watch::watchFontRead(){
  if( mp_isChangeFont == false && mp_scl_last_time != now() ){
    uint8_t num = 0;

    m_onColon_disp = true; //コロン表示開始フラグ

    if( day() != mp_prev_ymd ){
      m_changeYMD = true;
      mp_prev_ymd = day();
    }

    //時刻文字をフォント変換
    mp_now_hour1 = hour() / 10; //時、十の位
    if( mp_now_hour1 != mp_prev_hour1 ){
      num = 0;
      mp_watch_font_count[num] = mp_now_hour1 * 2; //全角は8x16フォント２つ分なので、フォントカウントを2倍進める
      mp_prev_hour1 = mp_now_hour1;
      mp_isStartScrolle[num] = true;
    }
    mp_now_hour2 = hour() % 10; //時、一の位
    if( mp_now_hour2 != mp_prev_hour2 ){
      num = 1;
      mp_watch_font_count[num] = mp_now_hour2 * 2;
      mp_prev_hour2 = mp_now_hour2;
      mp_isStartScrolle[num] = true;
    }
    mp_now_min1 = minute() / 10; //分、十の位
    if( mp_now_min1 != mp_prev_min1 ){
      num = 2;
      mp_watch_font_count[num] = mp_now_min1 * 2;
      mp_prev_min1 = mp_now_min1;
      mp_isStartScrolle[num] = true;
    }
    mp_now_min2 = minute() % 10; //分、一の位
    if( mp_now_min2 != mp_prev_min2 ){
      num = 3;
      mp_watch_font_count[num] = mp_now_min2 * 2;
      mp_prev_min2 = mp_now_min2;
      mp_isStartScrolle[num] = true;
    }
    mp_now_sec1 = second() / 10; //秒、十の位
    if( mp_now_sec1 != mp_prev_sec1 ){
      num = 4;
      mp_watch_font_count[num] = mp_now_sec1 * 2;
      mp_prev_sec1 = mp_now_sec1;
      mp_isStartScrolle[num] = true;
    }
    mp_now_sec2 = second() % 10; //秒、一の位
    if( mp_now_sec2 != mp_prev_sec2 ){
      num = 5;
      mp_watch_font_count[num] = mp_now_sec2 * 2;
      mp_prev_sec2 = mp_now_sec2;
      mp_isStartScrolle[num] = true;
    }

    for( num = 0; num < MAX_TXT_NUM; num++ ){ //フォント変換
      scl_set[num].full_or_half = SFR.convSjisToFontInc(mp_watch_sjis_buf[num], mp_watch_sjis_len[num], &mp_watch_font_count[num], mp_watch_font_buf[num]);
    }
    mp_scl_last_time = now();
  }else if( mp_isChangeFont == true ){
    if( now() - mp_font_change_last_time > 2 ){
      mp_isChangeFont = false;
    }
  }
}
//********** scrolleWatch and colon ************************
void ILI9341Watch::dispNormalWatchCl(){
  ILI9341Watch::dispNormalWatch();
  ILI9341Watch::displayColon2();
}
//************************************************************
void ILI9341Watch::dispNormalWatch(){
  ILI9341Watch::watchFontRead();
  uint8_t num = 0;
  for( num = 0; num < MAX_TXT_NUM; num++ ){
    if( mp_isStartScrolle[num] ){
      LCD.display8x16Font( font[num], 2, mp_watch_font_buf[num] ); //全角の場合は 2
      mp_isStartScrolle[num] = false;
    }
  }
}
//********** scrolleWatch and colon ************************
void ILI9341Watch::scrolleWatchCl(){
  ILI9341Watch::scrolleWatch();
  ILI9341Watch::displayColon2();
}
//************************************************************
void ILI9341Watch::scrolleWatch(){
  ILI9341Watch::watchFontRead();

  uint8_t num = 0; //時、十の位文字スクロール
  if( mp_isStartScrolle[num] ){
    if( LCD.scrolle8x16fontInc( font[num], scl_set[num], mp_watch_sjis_len[num], mp_watch_font_buf[num] ) ){
      mp_isStartScrolle[num] = false; //一文字スクロール後、スクロールストップ
    }
  }
  num = 1; //時、一の位文字スクロール
  if( mp_isStartScrolle[num] ){
    if( LCD.Yscrolle8x16fontInc( font[num], scl_set[num], mp_watch_sjis_len[num], mp_watch_font_buf[num] ) ){
      mp_isStartScrolle[num] = false;
    }
  }
  num = 2; //分、十の位文字スクロール
  if( mp_isStartScrolle[num] ){
    if( LCD.reverseScrolle8x16fontInc( font[num], scl_set[num], mp_watch_sjis_len[num], mp_watch_font_buf[num] ) ){
      mp_isStartScrolle[num] = false;
    }
  }
  num = 3; //分、一の位文字スクロール
  if( mp_isStartScrolle[num] ){
    if( LCD.YdownScrolle8x16fontInc( font[num], scl_set[num], mp_watch_sjis_len[num], mp_watch_font_buf[num] ) ){
      mp_isStartScrolle[num] = false;
    }
  }
  num = 4; //秒、十の位文字スクロール
  if( mp_isStartScrolle[num] ){
    if( LCD.scrolle8x16fontInc( font[num], scl_set[num], mp_watch_sjis_len[num], mp_watch_font_buf[num] ) ){
      mp_isStartScrolle[num] = false;
    }
  }
  num = 5; //秒、一の位文字スクロール
  if( mp_isStartScrolle[num] ){
    if( LCD.Yscrolle8x16fontInc( font[num], scl_set[num], mp_watch_sjis_len[num], mp_watch_font_buf[num] ) ){
      mp_isStartScrolle[num] = false;
    }
  }
}
//************ 時刻コロン表示 ********************************
void ILI9341Watch::displayColon(uint16_t colon1_sj_len, uint16_t colon2_sj_len, uint8_t colon1_buf[][ 16 ], uint8_t colon2_buf[][ 16 ]){
  if( m_onColon_disp ){
    LCD.display8x16Font(colon1_font, colon1_sj_len, colon1_buf);
    LCD.display8x16Font(colon2_font, colon2_sj_len, colon2_buf);
    m_onColon_disp = false;
    mp_colon_last_time = millis();
  }else if( millis() - mp_colon_last_time > 500 ){
    LCD.drawRectangleFill( colon1_font.x0, colon1_font.y0, colon1_font.x0 + colon1_font.Xsize * 16, colon1_font.y0 + colon1_font.Ysize * 16, colon1_font.bg_red, colon1_font.bg_green, colon1_font.bg_blue);
    LCD.drawRectangleFill( colon2_font.x0, colon2_font.y0, colon2_font.x0 + colon2_font.Xsize * 16, colon2_font.y0 + colon2_font.Ysize * 16, colon2_font.bg_red, colon2_font.bg_green, colon2_font.bg_blue);
    mp_colon_last_time = millis();
  }
}
//************ 時刻コロン表示 ********************************
void ILI9341Watch::displayColon2(){
  uint8_t colon_fnt2[][16] = {
    0,
    0,
    0b00111000,
    0b01111100,
    0b01111100,
    0b01111100,
    0b00111000,
    0,
    0,
    0b00111000,
    0b01111100,
    0b01111100,
    0b01111100,
    0b00111000,
    0,
    0
  };
  if( m_onColon_disp ){
    LCD.display8x16Font(colon1_font, 1, colon_fnt2);
    LCD.display8x16Font(colon2_font, 1, colon_fnt2);
    m_onColon_disp = false;
    mp_colon_last_time = millis();
  }else if( millis() - mp_colon_last_time > 500 ){
    LCD.drawRectangleFill( colon1_font.x0, colon1_font.y0, colon1_font.x0 + colon1_font.Xsize * 8, colon1_font.y0 + colon1_font.Ysize * 16, colon1_font.bg_red, colon1_font.bg_green, colon1_font.bg_blue);
    LCD.drawRectangleFill( colon2_font.x0, colon2_font.y0, colon2_font.x0 + colon2_font.Xsize * 8, colon2_font.y0 + colon2_font.Ysize * 16, colon2_font.bg_red, colon2_font.bg_green, colon2_font.bg_blue);
    mp_colon_last_time = millis();
  }
}
//*************************************
void ILI9341Watch::getNTP(){
  time_t tmp_time = EWG.getNtpTime();
  Serial.println(F("NTP Getting..."));
  Serial.printf("seconds since the epoch = %lu\r\n", tmp_time );

  if( tmp_time < 1000 ){
    NtpStatus = ConnectFailed;
    Serial.println(F( "NTP Get Failed" ));
  }else{
    NtpStatus = ConnectOK;
    setTime( tmp_time );
    Serial.printf( "NTP Get OK! %02d:%02d\r\n", hour(), minute() );
  }
}
//*************************************
void ILI9341Watch::getNTPserverSel( int timezone, const char *ntp_server_name, uint32_t interval ){
  if( (m_isNtp_first_get == true) || ((millis() - mp_ntp_get_last_time) > interval) ){
    ILI9341Watch::getNTPserverSel( timezone, ntp_server_name );
    mp_ntp_get_last_time = millis();
    m_isNtp_first_get = false;
  }
}
//*************************************
void ILI9341Watch::getNTPserverSel( int timezone, const char *ntp_server_name ){
  delay(2000); //WiFi接続後、ブランクが必要。
  NtpStatus = Connecting;
  ntp_msg_status = (int)Connecting;

  Serial.println(F("NTP Getting Wait"));
  Serial.print(F("WiFi.status = ")); Serial.println( WiFi.status() );

  /*
  WiFi.status()結果は、Arduino-ESP32 WiFiライブラリ中の WiFiType.h ファイルに記載されている。
  
  typedef enum {
      WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
      WL_IDLE_STATUS      = 0,
      WL_NO_SSID_AVAIL    = 1,
      WL_SCAN_COMPLETED   = 2,
      WL_CONNECTED        = 3,
      WL_CONNECT_FAILED   = 4,
      WL_CONNECTION_LOST  = 5,
      WL_DISCONNECTED     = 6
  } wl_status_t;
  */

  int16_t wifi_state = WiFi.status();
  if( wifi_state == WL_CONNECTED ){
    EWG.ntpServerInit( timezone, ntp_server_name );
    delay(1000);
    bool isNtpOK = EWG.getNtpServerSelect( timezone ); //NTPサーバーと接続できなかった場合、他のNTPサーバーと接続できるか試す関数

    if( isNtpOK ){
      NtpStatus = ConnectOK;
      ntp_msg_status = (int)ConnectOK;
      Serial.printf( "NTP Get OK! %02d:%02d\r\n", hour(), minute() );
    }else{
      NtpStatus = ConnectFailed;
      ntp_msg_status = (int)ConnectFailed;
      Serial.println(F( "NTP Get Failed" ));
    }
  }else{
    NtpStatus = ConnectFailed;
    delay(30); //マルチタスクでメッセージ表示させるためにはこのdelayが無いと、正常にディスプレイに表示してくれない。
    ntp_msg_status = (int)ConnectFailed;
    Serial.println(F("WiFi Disconnected NTP failed"));
  }
}
//*************************************
boolean ILI9341Watch::getNTP2(uint32_t get_interval){
  if( (m_isNtp_first_get == true) || ((millis() - mp_ntp_get_last_time) > get_interval) ){
    ILI9341Watch::getNTP();
    mp_ntp_get_last_time = millis();
    m_isNtp_first_get = false;
    return true;
  }
  return false;
}
//********** 時刻文字列フォント変換 *****************
void ILI9341Watch::watchStrFontConv(uint8_t num, String str){
  mp_watch_font_count[num] = 0;
  mp_watch_sjis_len[num] = SFR.convStrToSjis( str, mp_watch_sjis_buf[num]);
  scl_set[num].full_or_half = SFR.convSjisToFontInc(mp_watch_sjis_buf[num], mp_watch_sjis_len[num], &mp_watch_font_count[num], mp_watch_font_buf[num]);
}
//********** 年月日曜日表示 *************************
void ILI9341Watch::displayYMDW(){
  if( m_changeYMD ){
    //Timeライブラリ weekday()関数は1～7の範囲数値を返す
    char week_c[7][4] = { "日", "月", "火", "水", "木", "金", "土" };
    String ymd_str = "";
    char month_chr[ 3 ] = {}, day_chr[ 3 ] = {};
    uint8_t ymd_buf[40][16] = {};
  
    sprintf( month_chr, "%2d", month() );
    sprintf( day_chr, "%02d", day() );
    ymd_str = String( year()) + "年";
    ymd_str += String( month_chr) + "月";
    ymd_str += String( day_chr) + '(';
    ymd_str += String( week_c[ weekday() - 1 ] ) + "曜日)";
    Serial.println(ymd_str);
    uint16_t ymd_len = SFR.convStrToFont( ymd_str, ymd_buf );

    LCD.display8x16Font( ymd_font, ymd_len, ymd_buf );
    m_changeYMD = false;
  }
}
//****************************************
void ILI9341Watch::watchDispReset(){
  mp_prev_ymd = -1;
  mp_prev_hour1 = -1, mp_prev_hour2 = -1;
  mp_now_hour1 = -1, mp_now_hour2 = -1;
  mp_prev_min1 = -1, mp_prev_min2 = -1;
  mp_now_min1 = -1, mp_now_min2 = -1;
  mp_prev_sec1 = -1, mp_prev_sec2 = -1;
  mp_now_sec1 = -1, mp_now_sec2 = -1;
  mp_isChangeFont = true;
  mp_font_change_last_time = now();
  mp_scl_last_time = 0;
}

}  // namespace mynamespace
