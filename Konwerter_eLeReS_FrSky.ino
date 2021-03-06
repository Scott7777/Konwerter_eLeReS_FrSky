/*
  @author   Scott

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "Konwerter.h"

#ifdef DEBUG
char text[50];
#endif
Stream* port;
SoftwareSerial* FrskyData;
SoftwareSerial* eLeReSData;
uint32_t frame1Time;
uint32_t frame2Time;
uint32_t frame3Time;
struct eLeReS_data eLeReS;
String streLeReS;

//zmienne do sprzwdzwnia czy dalej nadawane są te parametry
unsigned int RSSI_OK;
unsigned int RCQ_OK;
unsigned int uRX_OK;
unsigned int tRX_OK;
unsigned int STX_OK;
unsigned int aRX_OK;
unsigned int uTX_OK;
unsigned int tTX_OK;
unsigned int P_OK;
unsigned int TRYB_OK;
unsigned int HD_OK;
unsigned int FIX_OK;
unsigned int SAT_OK;
unsigned int KURS_OK;
unsigned int v_OK;
unsigned int h_OK;
unsigned int Pos_OK;

void setup()
{
  FrskyData = new SoftwareSerial(Pin_FrSky, Pin_FrSky); //port dla danych frsky, na tym pinie nadajemy telemetrie
  port = FrskyData;
  FrskyData->begin(Baud_FrSky); //prędkość portu wysyłania telemetrii
  pinMode(Pin_FrSky, OUTPUT);

  eLeReSData = new SoftwareSerial(Pin_eLeReS, Pin_eLeReS); //port do nasłuchiwania eLeReSa
  port = eLeReSData;
  //58823 baud
  eLeReSData->begin(Baud_eLeReS); //prędkość portu eLeReSa
  pinMode(Pin_eLeReS, INPUT);
#ifdef DEBUG
  Serial.begin(57600); //port sprzętowy do komunikatów debug.
  Serial.println("Konwerter telemetrii eLeReS -> FrSky by Scott (rc-fpv.pl)");
#endif
  pinMode(Pin_Led, OUTPUT);
}


void sendAllData() //tworzenie kompletnej ramki danych do wysłania
{
  uint32_t currentTime = millis();
//  if (currentTime > frame3Time) // Sent every 5s (5000ms)
//  {
//    frame3Time = currentTime + 5000;
//    frame2Time = currentTime + 800; // Postpone frame 2 to next cycle
//    frame1Time = currentTime + 200; // Postpone frame 1 to next cycle
//
//    //sendUserData (FRSKY_TEMP1, eLeReS.tRX);
//    //sendUserData (FRSKY_TEMP2, eLeReS.tTX); //nie wysyłam temperatury nadajnika - nie jest to telemetria z modelu
//  }
//  else 
  if (currentTime > frame2Time) // Sent every 800ms
  {
    frame2Time = currentTime + 800;
    frame1Time = currentTime + 200; // Postpone frame 1 to next cycle

    sendUserData (FRSKY_TEMP1, eLeReS.tRX);
    sendUserData (FRSKY_GPS_ALT_B, eLeReS.h);
    sendUserData (FRSKY_GPS_SPEED_B, eLeReS.v);
    sendUserData (FRSKY_GPS_COURSE_B, eLeReS.KURS);
    sendUserData (FRSKY_GPS_LONG_B, eLeReS.LonB);
    sendUserData (FRSKY_GPS_LONG_A, eLeReS.LonA);
    sendUserData (FRSKY_GPS_LAT_B, eLeReS.LatB);
    sendUserData (FRSKY_GPS_LAT_A, eLeReS.LatA);
    sendUserData (FRSKY_GPS_HDop, eLeReS.HD);
    sendUserData (FRSKY_ALT_B, eLeReS.b_h_B);
    sendUserData (FRSKY_ALT_A, eLeReS.b_h_A);
    //sendUserData (FRSKY_RPM, 60);
    sendUserData (FRSKY_FUEL, eLeReS.FUEL);
   
    
  }
  else if (currentTime > frame1Time) // Sent every 200ms. Według er9x-frsky.cpp:// A1/A2/RSSI values // From a scope, this seems to be sent every about every 35mS 
  {
    frame1Time = currentTime + 200;
    
    sendLinkData(eLeReS.uRX, eLeReS.aRX, eLeReS.RSSI, eLeReS.RCQ);
    Parametry_OK(10); //zanik parametru telemetrii wykrywam po 10x200ms = 2s
  }
}

int ObliczFuel() //konwersja napięcia pakietu na wskaźnik fuel
{
  float fuel;
  float BAT_MIN = 85;
  float BAT_MAX = 126;

  fuel = eLeReS.uRX - BAT_MIN;
  fuel = fuel * (100 / (BAT_MAX - BAT_MIN));
  if (fuel < 0) fuel = 0;
  return fuel;
}

void sendUserData(uint8_t id, int16_t val) //wysłanie pojedyńczego pakietu USERDATA
{
  if (FrskyData != NULL)
  {

    FrskyData->write (0x7E);
    FrskyData->write (0xFD);
    FrskyData->write (0x04);
    FrskyData->write (0x5E);
    FrskyData->write (id);
    SendDValue(val);
    FrskyData->write (0x5E);   // End of frame
    FrskyData->write ((uint8_t)0x00);
    FrskyData->write ((uint8_t)0x00);
    FrskyData->write (0x7E);   // End of frame

    if (FrskyData != NULL) FrskyData->flush();
    //sprintf(text, "Wyslana ramka frsky UserData:%d, %d",id,val );
    //Serial.println(text);
  }
}

void sendLinkData(uint8_t A1, uint8_t A2, uint8_t Rssi, uint8_t Rcq) //wysłanie pakietu LINKDATA (A1,A2,RSSI,RCQ)
{
  if (FrskyData != NULL)
  {
    FrskyData->write (0x7E);
    FrskyData->write (0xFE);
    SendLValue(A1);
    SendLValue(A2);
    SendLValue(Rssi);
    SendLValue(Rcq);
    FrskyData->write ((uint8_t)0x00);
    FrskyData->write ((uint8_t)0x00);
    FrskyData->write ((uint8_t)0x00);
    FrskyData->write ((uint8_t)0x00);
    FrskyData->write (0x7E);   // End of frame

    if (FrskyData != NULL) FrskyData->flush();
    //sprintf(text, "Wyslana ramka frsky LinkData RSSI:%d",Rssi );
    //Serial.println(text);
  }
}

void SendLValue(uint8_t val)
{
    if (val == 0x7E) {
      FrskyData->write (0x7D);
      FrskyData->write (0x5E);
    }
    else if (val == 0x7D) {
      FrskyData->write (0x7D);
      FrskyData->write (0x5D);
    }
    else
      FrskyData->write (val);
}

void SendDValue(int16_t val)
{
  int8_t d[2];
  int i;

  d[0] =  val & 0x00ff;
  d[1] = (val & 0xff00) >> 8;

  for (i = 0; i < 2; i++) {
    if (d[i] == 0x5E) {
      FrskyData->write (0x5D);
      FrskyData->write (0x3E);
    }
    else if (d[i] == 0x5D) {
      FrskyData->write (0x5D);
      FrskyData->write (0x3D);
    }
    else if (d[i] == 0x7E) {
      FrskyData->write (0x7D);
      FrskyData->write (0x5E);
    }
    else if (d[i] == 0x7D) {
      FrskyData->write (0x7D);
      FrskyData->write (0x5D);
    }
    else
      FrskyData->write (d[i]);
  }
}

void readLRS() //czytanie eLeReSa obliczenia i pakowanie do tablicy
{
  String str;
  String tmp;
  String wynik;
  String nazwa;
  String wartosc;
  float lat;
  float lon;

  if (eLeReSData->available() > 0) {
  delay (10);
    if (streLeReS != "")
      str = streLeReS;
    else
      str = eLeReSData->readStringUntil('\n');

#ifdef DEBUG
    Serial.print ("rcv-eLeReS.Full_string: ");
    Serial.println(str);
#endif
    str.replace(", ", ",");
    str.replace("\r", "");
    //blink1();
    for (uint8_t x = 0; x < 10; x++)
    {
      String xval = getValue(str, ' ', x); //wydzielenie pary parametr=wartosc
      if (xval != NULL)
      {
        nazwa = getValue(xval, '=', 0);
        wartosc = getValue(xval, '=', 1);

        if ((nazwa == "RSSI" or nazwa == "SSI") and wartosc.length() == 3) {
          eLeReS.RSSI = wartosc.toInt();
          RSSI_OK = 0; //zerowanie licznika poprawności RSSI
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.RSSI:%d ", eLeReS.RSSI);
          Serial.print(text);
#endif
        } else if (nazwa == "RCQ" and wartosc.length() == 3) {
          eLeReS.RCQ = wartosc.toInt() *2;
          RCQ_OK = 0;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.RCQ:%d ", eLeReS.RCQ);
          Serial.print(text);
#endif
        } else if (nazwa == "U" and wartosc.length() == 5) {
          eLeReS.uRX = atof (wartosc.c_str()) * 10;
          uRX_OK = 0;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.uRX:%d ", eLeReS.uRX);
          Serial.print(text);
#endif
        } else if (nazwa == "T" and (wartosc.length() == 4 or wartosc.length() == 5)) {
          eLeReS.tRX = wartosc.toInt();
          tRX_OK = 0;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.tRX:%d ", eLeReS.tRX);
          Serial.print(text);
#endif
        } else if (nazwa == "I" and wartosc.length() == 5) {
          eLeReS.aRX = atof (wartosc.c_str()) * 10;
          aRX_OK = 0;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.aRX:%d ", eLeReS.aRX);
          Serial.print(text);
#endif
        } else if (nazwa == "UTX" and wartosc.length() == 5) {
          eLeReS.uTX = atof (wartosc.c_str()) * 10;
          uTX_OK = 0;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.uTX:%d ", eLeReS.uTX);
          Serial.print(text);
#endif
        } else if (nazwa == "STX" and wartosc.length() == 3) {
          eLeReS.STX = wartosc.toInt();
          STX_OK = 0;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.STX:%d ", eLeReS.STX);
          Serial.print(text);
#endif
        } else if (nazwa == "TTX" and (wartosc.length() == 4 or wartosc.length() == 5)) {
          eLeReS.tTX = wartosc.toInt();
          tTX_OK = 0;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.tTX:%d ", eLeReS.tTX);
          Serial.print(text);
#endif
        } else if (nazwa == "P" and wartosc.length() == 5) {
          eLeReS.P = (atof (wartosc.c_str()) + 50000) / 100;
          P_OK = 0;
#ifdef DEBUG
          Serial.print("rcv-eLeReS.P:");
          Serial.print(eLeReS.P);
          Serial.print(" ");
#endif
        } else if (nazwa == "F" and wartosc.length() == 2) {
          eLeReS.TRYB = wartosc.toInt();
          TRYB_OK = 0;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.TRYB:%d ", eLeReS.TRYB);
          Serial.print(text);
#endif
        } else if (nazwa == "HD" and wartosc.length() == 4) {
          eLeReS.HD = atof (wartosc.c_str()) * 10; //?
          HD_OK = 0;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.HD:%d ", eLeReS.HD);
          Serial.print(text);
#endif
        } else if (nazwa == "f" and wartosc.length() == 1) {
          eLeReS.FIX = wartosc.toInt();
          FIX_OK = 0;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.FIX:%d ", eLeReS.FIX);
          Serial.print(text);
#endif
        } else if (nazwa == "s" and wartosc.length() == 2) {
          eLeReS.SAT = wartosc.toInt();
          SAT_OK = 0;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.SAT:%d ", eLeReS.SAT);
          Serial.print(text);
#endif
        } else if (nazwa == "c" and wartosc.length() == 3) {
          eLeReS.KURS = wartosc.toInt();
          KURS_OK = 0;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.c:%d ", eLeReS.KURS);
          Serial.print(text);
#endif
        } else if (nazwa == "v" and wartosc.length() == 3) {
          eLeReS.v = wartosc.toInt();
          v_OK = 0;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.v:%d ", eLeReS.v);
          Serial.print(text);
#endif
        } else if (nazwa == "h" and wartosc.length() == 4) {
          eLeReS.h = wartosc.toInt();
          h_OK = 0;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.h:%d ", eLeReS.h);
          Serial.print(text);
#endif
        } else if ((nazwa == "Pos" or nazwa == "os") and wartosc.length() == 22) {
          //pobranie lattitude
          tmp = getValue(wartosc, ',', 0);
          lat = atof (tmp.c_str());
          eLeReS.LatB = (uint16_t)lat;
          lat = (lat - (float)eLeReS.LatB) * 60.0;
          eLeReS.LatB = eLeReS.LatB * 100 + (uint16_t)lat;
          eLeReS.LatA = (uint16_t)round((lat - (uint16_t)lat) * 10000.0);
          //pobranie longtitude
          tmp = getValue(wartosc, ',', 1);
          lon = atof (tmp.c_str());
          eLeReS.LonB = (uint16_t)lon;
          lon = (lon - (float)eLeReS.LonB) * 60.0;
          eLeReS.LonB = eLeReS.LonB * 100 + (uint16_t)lon;
          eLeReS.LonA = (uint16_t)round((lon - (uint16_t)lon) * 10000.0);
          Pos_OK = 0;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.LatB:%d ", eLeReS.LatB);
          Serial.print(text);
          sprintf(text, "rcv-eLeReS.LatA:%d ", eLeReS.LatA);
          Serial.print(text);
          sprintf(text, "rcv-eLeReS.LonB:%d ", eLeReS.LonB);
          Serial.print(text);
          sprintf(text, "rcv-eLeReS.LonA:%d ", eLeReS.LonA);
          Serial.print(text);
#endif
        }
      }
    }
    eLeReS.FUEL = ObliczFuel();
    float sealevelPressure = 101325;
    if (eLeReS.P == 0) 
    {
      eLeReS.b_h_B=0; 
      eLeReS.b_h_A=0;
    } 
    else 
    {
      float alt = 44330 * (1.0 - pow((eLeReS.P * 100) / sealevelPressure, 0.1903));
      eLeReS.b_h_B = (uint16_t)alt;
      eLeReS.b_h_A = abs((int16_t)round((alt - eLeReS.b_h_B) * 100.0));
    }

#ifdef DEBUG
    Serial.println();
#endif
    if (eLeReSData != NULL) eLeReSData->flush();
  }
}

void Parametry_OK(int okres)
{
  RSSI_OK++;  //co cykl zwiększam o jeden wartość parametru
  RCQ_OK++;
  uRX_OK++;
  tRX_OK++;
  STX_OK++;
  aRX_OK++;
  uTX_OK++;
  tTX_OK++;
  P_OK++;
  TRYB_OK++;
  HD_OK++;
  FIX_OK++;
  SAT_OK++;
  KURS_OK++;
  v_OK++;
  h_OK++;
  Pos_OK++;

  if (RSSI_OK > okres)  eLeReS.RSSI = 0; //jeśli parametr osiągnie pewną wartość - oznacza to , że przez dłuższy okres nie był odświeżony - znaczy nie jest nadawany w telemetrii. Zeruję go.
  if (RCQ_OK > okres) eLeReS.RCQ = 0;
  if (uRX_OK > okres)   eLeReS.uRX = 0;
  if (tRX_OK > okres)   eLeReS.tRX = 0;
  if (STX_OK > okres)   eLeReS.STX = 0;
  if (aRX_OK > okres)   eLeReS.aRX = 0;
  if (uTX_OK > okres)   {
    eLeReS.uTX = 0;
    eLeReS.FUEL = 0;
  }
  if (tTX_OK > okres)   eLeReS.tTX = 0;
  if (P_OK > okres)   eLeReS.P = 0;
  if (TRYB_OK > okres)   eLeReS.TRYB = 0;
  if (HD_OK > okres)   eLeReS.HD = 0;
  if (FIX_OK > okres)   eLeReS.FIX = 0;
  if (SAT_OK > okres)   eLeReS.SAT = 0;
  if (KURS_OK > okres)   eLeReS.KURS = 0;
  if (v_OK > okres)   eLeReS.v = 0;
  if (h_OK > okres)   eLeReS.h = 0;
  //if (Pos_OK > okres)  {eLeReS.LonA = 0; eLeReS.LonB = 0; eLeReS.LatA = 0; eLeReS.LatB = 0;}
  
  //sprintf(text, "h_OK:%d eLeReS.h:%d",h_OK,eLeReS.h );
  //Serial.println(text);
}

void testeLeReS()
{
  if (Serial.available() > 0) {

    String str = Serial.readStringUntil('\n');
    String nazwa = getValue(str, ':', 0);
    String wartosc = getValue(str, ':', 1);
    if (nazwa == "TEST") streLeReS = wartosc;
    if (wartosc == "STOP") streLeReS = "";
  }
}

void loop()
{
#ifdef DEBUG
  testeLeReS();
#endif
  readLRS();
  //delay(50);
  sendAllData();
}


