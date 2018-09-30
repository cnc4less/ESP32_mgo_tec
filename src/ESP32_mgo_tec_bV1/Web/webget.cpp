/*
  webget.cpp
  Beta version 1.0.11

Copyright (c) 2017 Mgo-tec

This library is used by the Arduino IDE(Tested in ver1.8.2).

Reference Blog --> https://www.mgo-tec.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Reference LGPL-2.1 license statement --> https://opensource.org/licenses/LGPL-2.1   

WiFi.h - Included WiFi library for esp32
Based on WiFi.h from Arduino WiFi shield library.
Copyright (c) 2011-2014 Arduino.  All right reserved.
Modified by Ivan Grokhotkov, December 2014
Licensed under the LGPL-2.1

WiFiClientSecure.h
Copyright (c) 2011 Adrian McEwen.  All right reserved.
Additions Copyright (C) 2017 Evandro Luis Copercini.
Licensed under the LGPL-2.1

WiFiUdp.h
Copyright (c) 2008 Bjoern Hartmann
Licensed under the MIT.

*/

#include "ESP32_mgo_tec_bV1/Web/webget.h"

// In the webget.cpp file
namespace mgo_tec_esp32_bv1 {

// Definition of functions is within scope of the namespace.

static IPAddress mp_NtpServerIP;
static const int mp_NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
static byte mp_packetBuffer[ mp_NTP_PACKET_SIZE ]; //buffer to hold incoming and outgoing packets
static int mp_timeZone = 9; // Tokyo
static WiFiUDP mp_Udp;
static const unsigned int mp_localPort = 8888;  // local port to listen for UDP packets

//********AP(Router) Connection****
bool WebGetClass::wifiAPconnect(const char *ssid, const char *password){
  //Serial.begin(115200); //※二重使用厳禁！　WiFi STA 接続できなくなる。

  Serial.println();
  Serial.println(F("Connecting Wifi..."));
  Serial.println(ssid);
/*
  WiFi.begin(ssid, password);

  uint32_t WIFItimeOut = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if( millis() - WIFItimeOut > 20000 ) return false;
  }
  Serial.println();
  Serial.println(F("WiFi connected"));
  delay(1000);
  Serial.println(WiFi.localIP());
*/
  WiFi.disconnect(true);
  delay(1000);
  WiFiMulti wifiMulti;
  wifiMulti.addAP(ssid, password);

  if(wifiMulti.run() == WL_CONNECTED) {
      Serial.println("");
      Serial.println(F("WiFi connected"));
      Serial.println(F("IP address: "));
      Serial.println(WiFi.localIP());
  }
  if( WiFi.status() >= WL_CONNECT_FAILED ){
    return false;
  }

  return true;
}
//******** NTP server init *******
void WebGetClass::ntpServerInit(int timezone, const char *NtpServerName){
  mp_timeZone = timezone;
  WiFi.hostByName(NtpServerName, mp_NtpServerIP);
  Serial.print(NtpServerName);
  Serial.print(": ");
  Serial.println(mp_NtpServerIP);

  mp_Udp.begin(mp_localPort);
  delay(2000); //UDP.begin後、2秒程ブランクが必要。
}
//********NTP init set TimeLibrary******
void WebGetClass::getNtpTimeInit(int timezone, const char *NtpServerName){
  WebGetClass::ntpServerInit(timezone, NtpServerName);
  
  setTime(EWG_Get_Ntp_Time());
  
  WebGetClass::getNtpServerSelect(timezone);
  
}
//*************** HTTP GET **********************************************
String WebGetClass::webGet(const char* host0, String target_ip, char char_tag, String Final_tag, String Begin_tag, String End_tag, String Paragraph){

  String ret_str = "";

  WiFiClient client2;

  if (client2.connect(host0, 80)) {
    Serial.print(host0); Serial.print(F("-------------"));
    Serial.println(F("connected"));
    Serial.println(F("--------------------WEB HTTP GET Request"));

    String str1 = "GET " + target_ip + " HTTP/1.1\r\n";
           str1 += "Host: " + String( host0 ) + "\r\n\0";
    String str2 = "Content-Type: text/html; charset=UTF-8\r\n";
           str2 += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n";
           str2 += "Content-Language: ja\r\n";
           str2 += "Accept-Language: ja\r\n";
           str2 += "Accept-Charset: UTF-8\r\n";
           str2 += "Connection: close\r\n\r\n\0"; //closeを使うと、サーバーの応答後に切断される。最後に空行必要

    client2.print( str1 );
    client2.print( str2 );

    Serial.println( str1 );
    Serial.println( str2 );
  }else {
    // if you didn't get a connection to the server2:
    Serial.println(F("connection failed"));
  }

  if(client2){
    String dummy_str;
    uint16_t from, to;
    Serial.println(F("--------------------WEB HTTP Response"));

    uint32_t Http_Time_Out = millis();

    while(client2.connected()){
      if((millis() - Http_Time_Out) > 60000L) break; //60seconds Time Out
      while (client2.available()) {
        if((millis() - Http_Time_Out) > 60000L) break; //60seconds Time Out
        if(dummy_str.indexOf(Final_tag) == -1){
          dummy_str = client2.readStringUntil(char_tag);
          if(dummy_str.indexOf(Begin_tag) >= 0){
            from = dummy_str.indexOf(Begin_tag) + Begin_tag.length();
            to = dummy_str.indexOf(End_tag);
            ret_str += Paragraph;
            ret_str += dummy_str.substring(from,to);
            ret_str += "  ";
          }
        }else{
          while(client2.available()){
            if((millis() - Http_Time_Out) > 60000L) break; //60seconds Time Out
            client2.read();
            yield();
          }
          client2.flush();
          delay(10);
          client2.stop();
          delay(10);
          Serial.println(F("--------------------Client Stop"));
          break;
        }
        yield();
      }
      yield();
    }
  }
  ret_str += "\0";
  ret_str.replace("&amp;","＆"); //XMLソースの場合、半角&が正しく表示されないので、全角に置き換える
  ret_str.replace("&#039;","\'"); //XMLソースの場合、半角アポストロフィーが正しく表示されないので置き換える

  if(client2){
    client2.flush();
    delay(10);
    client2.stop();
    delay(10);
    Serial.println(F("--------------------Client Stop"));
  }

  return ret_str;
}

//***************** SSL https GET *************************************************
String WebGetClass::httpsWebGet(const char* host1, String target_ip, char char_tag, String Final_tag, String Begin_tag, String End_tag, String Paragraph){
  String str = WebGetClass::httpsGet("\0", 0, host1, target_ip, char_tag, Final_tag, Begin_tag, End_tag, Paragraph);
  return str;
}

String WebGetClass::httpsWebGet(const char *root_ca, const char* host1, String target_ip, char char_tag, String Final_tag, String Begin_tag, String End_tag, String Paragraph){
  String str = WebGetClass::httpsGet(root_ca, 1, host1, target_ip, char_tag, Final_tag, Begin_tag, End_tag, Paragraph);
  return str;
}

String WebGetClass::httpsGet( const char *Root_Ca, uint8_t rca_set, const char* Host, String t_ip, char c_tag, String F_tag, String B_tag, String E_tag, String Pph ){
  int16_t wifi_state = WiFi.status();
  if( wifi_state != WL_CONNECTED ){
    return "※WiFi APに接続できません";
  }

  String ret_str1;
  WiFiClientSecure client;

  if( rca_set ==1 ){
    client.setCACert( Root_Ca );
    Serial.println( F("-------Root CA SET") );
  }else{
    Serial.println( F("------- Nothing Root CA") );
  }

  uint32_t time_out = millis();
  while( 1 ){
    /*インターネットが不意に切断されたときや、長時間接続している時には再接続できなくなる。
    再接続時、client.connect が true になるまで時間がかかる場合があるので、数回トライする必要がある。*/
    if ( client.connect( Host, 443 ) ){
      Serial.print( Host ); Serial.print( F("-------------") );
      Serial.println( F("connected") );
      Serial.println( F("-------Send HTTPS GET Request") );

      String str1 = String( F("GET ") );
             str1 += t_ip + F(" HTTP/1.1\r\n");
             str1 += F("Host: ");
             str1 += String( Host ) + F("\r\n");
             str1 += F("User-Agent: BuildFailureDetectorESP32\r\n");
             str1 += F("Connection: close\r\n\r\n"); //closeを使うと、サーバーの応答後に切断される。最後に空行必要
             str1 += "\0";

      client.print( str1 ); //client.println にしないこと。最後に改行コードをプラスして送ってしまう為
      client.flush(); //client出力が終わるまで待つ
      //log_v( "%s", str1.c_str() );
      //Serial.flush(); //シリアル出力が終わるまで待つ指令は、余分なdelayがかかってしまうので基本的に使わない
      break;
    }
    if( ( millis() - time_out ) > 20000 ){
      Serial.println( F("time out!") );
      Serial.println( F("Host connection failed.") );
      return "※Host に接続できません。";
    }
    delay(1);
  }

  time_out = millis();
  if( client ){
    String dummy_str;
    uint16_t from, to;
    Serial.println( F("-------Receive HTTPS Response") );

    while( client.connected() ){
      if( ( millis() - time_out ) > 60000 ){
        Serial.println( F("time out!"));
        Serial.println( F("Host HTTPS response failed.") );
        break;
      }
      while( client.available() ) {
        if( dummy_str.indexOf( F_tag ) == -1){
          dummy_str = client.readStringUntil( c_tag );
          if( dummy_str.indexOf( B_tag ) >= 0 ){
            from = dummy_str.indexOf( B_tag ) + B_tag.length();
            to = dummy_str.indexOf( E_tag );
            ret_str1 += Pph;
            ret_str1 += dummy_str.substring( from, to );
            ret_str1 += "  ";
          }
        }else{
          while( client.available() ){
            if( ( millis() - time_out ) > 60000 ) break; //60seconds Time Out
            client.read();
            //delay(1);
          }

          delay(10);
          client.stop();
          delay(10);
          Serial.println( F("-------Client Stop") );

          break;
        }
      }
    }
  }
  ret_str1 += "\0";
  ret_str1.replace( "&amp;", "＆" ); //XMLソースの場合、半角&が正しく表示されないので、全角に置き換える
  ret_str1.replace( "&#039;", "\'" ); //XMLソースの場合、半角アポストロフィーが正しく表示されないので置き換える
  ret_str1.replace( "&#39;", "\'" ); //XMLソースの場合、半角アポストロフィーが正しく表示されないので置き換える
  ret_str1.replace( "&apos;", "\'" ); //XMLソースの場合、半角アポストロフィーが正しく表示されないので置き換える
  ret_str1.replace( "&quot;", "\"" ); //XMLソースの場合、ダブルクォーテーションが正しく表示されないので置き換える

  if( ret_str1.length() < 20 ) ret_str1 = "※WEB GETできませんでした";

  if( client ){
    delay(10);
    client.stop();
    delay(10);
    Serial.println( F("-------Client Stop") );
  }

  return ret_str1;
}

//***************** Weather MyFont Number get *************************************************
void WebGetClass::weatherJfontNum(String str, uint8_t wDay, uint8_t Htime, uint8_t Fnum[3], uint8_t col[3][3]){
  uint8_t Sunny_red = 7, Sunny_green = 3, Sunny_blue = 0; //256bit color 晴れ
  uint8_t Cloudy_red = 3, Cloudy_green = 3, Cloudy_blue = 1; //256bit color 曇り
  uint8_t Rain_red = 0, Rain_green = 0, Rain_blue = 3; //256bit color 雨、大雨、暴風雨
  uint8_t Snow_red = 7, Snow_green = 7, Snow_blue = 3; //256bit color 雪
  uint8_t Thunder_red = 7, Thunder_green = 7, Thunder_blue = 0; //256bit color 雷
  uint8_t red = 0, green = 0, blue = 0;

  col[1][0] = 7; col[1][1] = 7; col[1][2] = 3;

  uint8_t fnt_num = 0;
  bool Single = true;

  if((str.indexOf("時々") >= 0) || (str.indexOf("一時") >= 0)){
    Single = false;
    Fnum[1] = 27;
  }else if(str.indexOf("後") >= 0){
    Single = false;
    Fnum[1] = 28;
  }else if(str.indexOf("時々") < 0 && str.indexOf("後") < 0){
    Single = true;
  }

  if(str.indexOf("晴") == 1){
    if((wDay == 0) && (Htime >= 15)){ //wDay = 0 今日、wDay = 1 明日
      red =  Thunder_red; green = Thunder_green; blue = Thunder_blue;
      fnt_num = 26;
    }else{
      red =  Sunny_red; green = Sunny_green; blue = Sunny_blue;
      fnt_num = 20;
    }
  }else if(str.indexOf("曇") == 1){
    red =  Cloudy_red; green = Cloudy_green; blue = Cloudy_blue;
    fnt_num = 21;
  }else if(str.indexOf("雨") == 1){
    red =  Rain_red; green = Rain_green; blue = Rain_blue;
    fnt_num = 22;
  }else if(str.indexOf("雪") == 1){
    red =  Snow_red; green = Snow_green; blue = Snow_blue;
    fnt_num = 24;
  }else if(str.indexOf("雷") == 1){
    red =  Thunder_red; green = Thunder_green; blue = Thunder_blue;
    fnt_num = 25;
  }else if((str.indexOf("暴風雨") == 1) || (str.indexOf("大雨") == 1)){
    red =  Rain_red; green = Rain_green; blue = Rain_blue;
    fnt_num = 23;
  }

  if(Single == true){
    Fnum[0] = 0;
    Fnum[1] = fnt_num;
    Fnum[2] = 0;
    col[1][0] = red; col[1][1] = green; col[1][2] = blue;
  }else{
    Fnum[0] = fnt_num;
    col[0][0] = red; col[0][1] = green; col[0][2] = blue;
  }

  if(Single == false){
    if(str.indexOf("晴") > 1){
      red =  Sunny_red; green = Sunny_green; blue = Sunny_blue;
      Fnum[2] = 20;
    }else if(str.indexOf("曇") > 1){
      red =  Cloudy_red; green = Cloudy_green; blue = Cloudy_blue;
      Fnum[2] = 21;
    }else if(str.indexOf("雨") > 1){
      red =  Rain_red; green = Rain_green; blue = Rain_blue;
      Fnum[2] = 22;
    }else if(str.indexOf("雪") > 1){
      red =  Snow_red; green = Snow_green; blue = Snow_blue;
      Fnum[2] = 24;
    }else if(str.indexOf("雷") > 1){
      red =  Thunder_red; green = Thunder_green; blue = Thunder_blue;
      Fnum[2] = 25;
    }else if((str.indexOf("暴風雨") > 1) || (str.indexOf("大雨") > 1)){
      red =  Rain_red; green = Rain_green; blue = Rain_blue;
      Fnum[2] = 23;
    }
    col[2][0] = red; col[2][1] = green; col[2][2] = blue;
  }
}

//***************** NTP time GET *************************************************
time_t EWG_Get_Ntp_Time(){
  while (mp_Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println(F("Transmit NTP Request"));
  EWG_Send_NTP_Packet(mp_NtpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = mp_Udp.parsePacket();
    if (size >= mp_NTP_PACKET_SIZE) {
      Serial.println(F("Receive NTP Response"));
      mp_Udp.read(mp_packetBuffer, mp_NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)mp_packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)mp_packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)mp_packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)mp_packetBuffer[43];
      return secsSince1900 - 2208988800UL + mp_timeZone * 3600UL;
    }
  }
  Serial.println(F("No NTP Response :-("));
  return 0; // return 0 if unable to get the time
}
//*************************NTP Time**************************************
void EWG_Send_NTP_Packet(IPAddress &address){
  memset(mp_packetBuffer, 0, mp_NTP_PACKET_SIZE);
  mp_packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  mp_packetBuffer[1] = 0;     // Stratum, or type of clock
  mp_packetBuffer[2] = 6;     // Polling Interval
  mp_packetBuffer[3] = 0xEC;  // Peer Clock Precision
  mp_packetBuffer[12]  = 49;
  mp_packetBuffer[13]  = 0x4E;
  mp_packetBuffer[14]  = 49;
  mp_packetBuffer[15]  = 52;         
  mp_Udp.beginPacket(address, 123); //NTP requests are to port 123
  mp_Udp.write(mp_packetBuffer, mp_NTP_PACKET_SIZE);
  mp_Udp.endPacket();
}
//************ NTP server 取得出来ない場合、別サーバーを選ぶ **********
bool WebGetClass::getNtpServerSelect(uint8_t timezone){
  bool ret = false;
  time_t tmp_time = EWG_Get_Ntp_Time();
  if( tmp_time < 600000 ){
    Serial.println(F("------ NTP time GET Try again"));
    const char *ntpServerName[7] = {
      "time.windows.com"
      "time.nist.gov",
      "time-a.nist.gov",
      "time-b.nist.gov",
      "time-nw.nist.gov",
      "time-a.timefreq.bldrdoc.gov",
      "time.windows.com"
    };

    for( int i=0; i<7; i++ ){
      if( tmp_time >= 600000 ) break;
      WebGetClass::ntpServerInit(timezone, ntpServerName[i]);
      delay(1000);
      tmp_time = EWG_Get_Ntp_Time();
    }

    if( tmp_time > 600000 ){
      Serial.println(F("NTP Server Get OK!"));
      setTime( tmp_time );
      ret = true;
    }else{
      Serial.println(F("------ ALL NTP Server Disconnection"));
      ret = false;
    }
  }else{
    Serial.println(F("NTP Server Get OK!"));
    setTime( tmp_time );
    ret = true;
  }
  return ret;
}
//************ NTP server 定期取得 **********
void WebGetClass::getNtpInterval(uint32_t interval){
  if((millis() - mp_LastNTPtime) > interval){
    time_t t = EWG_Get_Ntp_Time();
    if( t > 1000){
      setTime(t);
    }else{
      Serial.println(F("------Cannot get NTP server time"));
    }
    mp_LastNTPtime = millis();
  }
}

}  // namespace mynamespace
