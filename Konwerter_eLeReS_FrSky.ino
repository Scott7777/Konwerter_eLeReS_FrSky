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

Stream* port;
SoftwareSerial* FrskyData;
SoftwareSerial* eLeReSData;
uint32_t frame1Time;
uint32_t frame2Time;
uint32_t frame3Time;
struct eLeReS_data eLeReS;
unsigned int RSSI_OK;

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
  Serial.begin(19200); //port sprzętowy do komunikatów debug.
#endif
  pinMode(Pin_Led, OUTPUT);
}


void sendAllData() //tworzenie kompletnej ramki danych do wysłania
{
  uint32_t currentTime = millis();
  if (currentTime > frame3Time) // Sent every 5s (5000ms)
  {
    frame3Time = currentTime + 5000;
    frame2Time = currentTime + 200; // Postpone frame 2 to next cycle
    frame1Time = currentTime + 200; // Postpone frame 1 to next cycle

    //    sendIniData();
  }
  else if (currentTime > frame2Time) // Sent every 1s (1000ms)
  {
    frame2Time = currentTime + 1000;
    frame1Time = currentTime + 200; // Postpone frame 1 to next cycle

    sendUserData (FRSKY_TEMP1, eLeReS.tRX);
    sendUserData (FRSKY_TEMP2, eLeReS.tTX);
  }
  else if (currentTime > frame1Time) // Sent every 200ms
  {
    frame1Time = currentTime + 200;
    RSSI_OK++;
    if (RSSI_OK > 25)           //jeśli znika RSSI to kasujemy wszystkie wartośći z wyjątkiem pozycji z GPS
    { //jest ona wysyłana do wyłączenia aparatury
      //Czysc_eLeReS();           //zanik RSSI wykrywam po 25x200ms = 5s
    }
    sendLinkData(eLeReS.uRX, eLeReS.aRX, eLeReS.RSSI, eLeReS.RCQ);
    sendUserData (FRSKY_GPS_ALT, eLeReS.h);
    sendUserData (FRSKY_GPS_SPEED_B, eLeReS.v);
    sendUserData (FRSKY_GPS_COURSE_B, eLeReS.KURS);
    sendUserData (FRSKY_GPS_LONG_B, eLeReS.LonB);
    sendUserData (FRSKY_GPS_LONG_A, eLeReS.LonA);
    sendUserData (FRSKY_GPS_LAT_B, eLeReS.LatB);
    sendUserData (FRSKY_GPS_LAT_A, eLeReS.LatA);
    //sendUserData (FRSKY_RPM, 60);
    sendUserData (FRSKY_FUEL, eLeReS.FUEL);
  }
}

int ObliczFuel() //konwersja napięcia pakietu na wskaźnik fuel
{
  float fuel;
  char text[50];
  float BAT_MIN = 85;
  float BAT_MAX = 126;

  fuel = eLeReS.uRX - BAT_MIN;
  fuel = fuel * (100 / (BAT_MAX - BAT_MIN));
  if (fuel < 0) fuel = 0;
  return fuel;
}

void sendUserData(uint8_t id, int16_t val) //wysłanie pojedyńczego pakietu USERDATA
{
  int8_t d[2];
  int i;
  if (FrskyData != NULL)
  {
    d[0] =  val & 0x00ff;
    d[1] = (val & 0xff00) >> 8;

    FrskyData->write (0x7E);
    FrskyData->write (0xFD);
    FrskyData->write (0x04);
    //port->write (0x01);
    FrskyData->write (0x5E);
    FrskyData->write (id);

    for (i = 0; i < 2; i++) {
      if      (d[i] == 0x5E) {
        FrskyData->write (0x5D);
        FrskyData->write (0x3E);
      }
      else if (d[i] == 0x5D) {
        FrskyData->write (0x5D);
        FrskyData->write (0x3D);
      }
      else
        FrskyData->write (d[i]);
    }

    FrskyData->write (0x5E);   // End of frame
    FrskyData->write (0x7E);   // End of frame

    if (FrskyData != NULL) FrskyData->flush();
  }
}

void sendLinkData(uint8_t A1, uint8_t A2, uint8_t Rssi, uint8_t Rcq) //wysłanie pakietu LINKDATA (A1,A2,RSSI,RCQ)
{
  if (FrskyData != NULL)
  {
    FrskyData->write (0x7E);
    FrskyData->write (0xFE);
    FrskyData->write (A1);
    FrskyData->write (A2);
    FrskyData->write (Rssi);
    FrskyData->write (Rcq);
    FrskyData->write (0x7E);   // End of frame

    if (FrskyData != NULL) FrskyData->flush();
  }
}

void readLRS() //czytanie eLeReSa obliczenia i pakowanie do tablicy
{
  String str;
  String tmp;
  String wynik;
  String nazwa;
  String wartosc;
  char text[50];
  float lat;
  float lon;

  if (eLeReSData->available() > 0) {

    str = eLeReSData->readStringUntil('\n');
    //str = eLeReSData->readString();
#ifdef DEBUG
    Serial.print ("rcv-eLeReS.Full_string: ");
    Serial.println(str);
#endif
    str.replace(", ", ",");
    blink1();
    for (uint8_t x = 0; x < 20; x++)
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
          eLeReS.RCQ = wartosc.toInt();
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.RCQ:%d ", eLeReS.RCQ);
          Serial.print(text);
#endif
        } else if (nazwa == "U" and wartosc.length() == 5) {
          eLeReS.uRX = atof (wartosc.c_str()) * 10;
          eLeReS.FUEL = ObliczFuel();
          if (eLeReS.uRX > 124) eLeReS.uRX = 124; //błąd wyliczania namięcia powyżej 12,4v - do znalezienia
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.uRX:%d ", eLeReS.uRX);
          Serial.print(text);
          sprintf(text, "rcv-eLeReS.FUEL:%d ", eLeReS.FUEL);
          Serial.print(text);
#endif
        } else if (nazwa == "T" and (wartosc.length() == 5 or wartosc.length() == 6)) {
          eLeReS.tRX = wartosc.toInt();
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.tRX:%d ", eLeReS.tRX);
          Serial.print(text);
#endif
        } else if (nazwa == "I" and wartosc.length() == 5) {
          eLeReS.aRX = atof (wartosc.c_str()) * 10;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.aRX:%d ", eLeReS.aRX);
          Serial.print(text);
#endif
        } else if (nazwa == "UTX" and wartosc.length() == 5) {
          eLeReS.uTX = atof (wartosc.c_str()) * 10;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.uTX:%d ", eLeReS.uTX);
          Serial.print(text);
#endif
        } else if (nazwa == "STX" and wartosc.length() == 3) {
          eLeReS.STX = wartosc.toInt();
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.STX:%d ", eLeReS.STX);
          Serial.print(text);
#endif
        } else if (nazwa == "TTX" and (wartosc.length() == 5 or wartosc.length() == 6)) {
          eLeReS.tTX = wartosc.toInt();
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.tTX:%d ", eLeReS.tTX);
          Serial.print(text);
#endif
        } else if (nazwa == "P" and wartosc.length() == 5) {
          eLeReS.P = wartosc.toInt();
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.P:%d ", eLeReS.P);
          Serial.print(text);
#endif
        } else if (nazwa == "F" and wartosc.length() == 1) {
          eLeReS.TRYB = wartosc.toInt();
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.TRYB:%d ", eLeReS.TRYB);
          Serial.print(text);
#endif
        } else if (nazwa == "HD" and wartosc.length() == 4) {
          eLeReS.HDg = atof (wartosc.c_str()) * 10; //?
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.HDg:%d ", eLeReS.HDg);
          Serial.print(text);
#endif
        } else if (nazwa == "f" and wartosc.length() == 1) {
          eLeReS.FIX = wartosc.toInt();
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.FIX:%d ", eLeReS.FIX);
          Serial.print(text);
#endif
        } else if (nazwa == "s" and wartosc.length() == 2) {
          eLeReS.SAT = wartosc.toInt();
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.SAT:%d ", eLeReS.SAT);
          Serial.print(text);
#endif
        } else if (nazwa == "c" and wartosc.length() == 3) {
          eLeReS.KURS = wartosc.toInt();
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.c:%d ", eLeReS.KURS);
          Serial.print(text);
#endif
        } else if (nazwa == "v" and wartosc.length() == 3) {
          eLeReS.v = wartosc.toInt();
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.v:%d ", eLeReS.v);
          Serial.print(text);
#endif
        } else if (nazwa == "h" and wartosc.length() == 4) {
          eLeReS.h = wartosc.toInt();
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
    Serial.println();
  }
}

void Czysc_eLeReS()
{
  eLeReS.RSSI = NULL;
  eLeReS.RCQ = NULL;
  eLeReS.uRX = NULL;
  eLeReS.tRX = NULL;
  eLeReS.aRX = NULL;
  eLeReS.uTX = NULL;
  eLeReS.tTX = NULL;
  eLeReS.P = NULL;
  eLeReS.TRYB = NULL;
  eLeReS.HDg = NULL;
  eLeReS.FIX = NULL;
  eLeReS.SAT = NULL;
  eLeReS.KURS = NULL;
  eLeReS.v = NULL;
  eLeReS.h = NULL;
  eLeReS.FUEL = NULL;
}

void loop()
{
  readLRS();
  //delay(50);
  sendAllData();
}


