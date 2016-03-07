#include "Arduino.h"
#include "SoftwareSerial.h"
#include "Konwerter.h"

//#define DEBUG

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
  FrskyData = new SoftwareSerial(2, 2);
  port = FrskyData;
  FrskyData->begin(9600);
  pinMode(2, OUTPUT);

  eLeReSData = new SoftwareSerial(3, 4);
  port = eLeReSData;
  //58823 baud
  eLeReSData->begin(57600);
  pinMode(4, OUTPUT);
  pinMode(3, INPUT);
#ifdef DEBUG
  Serial.begin(9600);
#endif

  pinMode(13, OUTPUT);
}


void sendAllData()
{
  uint32_t currentTime = millis();
  if (currentTime > frame3Time) // Sent every 5s (5000ms)
  {
    frame3Time = currentTime + 2000;
    frame2Time = currentTime + 100; // Postpone frame 2 to next cycle
    frame1Time = currentTime + 100; // Postpone frame 1 to next cycle
    //    sendIniData();
  }
  else if (currentTime > frame2Time) // Sent every 1s (1000ms)
  {
    frame2Time = currentTime + 500;
    frame1Time = currentTime + 100; // Postpone frame 1 to next cycle

    sendUserData (FRSKY_TEMP1, eLeReS.tRX);
    sendUserData (FRSKY_TEMP2, eLeReS.tTX);
  }
  else if (currentTime > frame1Time) // Sent every 200ms
  {
    frame1Time = currentTime + 100;

    sendLinkData(eLeReS.uRX, eLeReS.aRX, eLeReS.RSSI, eLeReS.RCQ);
    sendUserData (FRSKY_GPS_ALT, eLeReS.h);
    sendUserData (FRSKY_GPS_SPEED_B, eLeReS.v);
    sendUserData (FRSKY_GPS_COURSE_B, eLeReS.KURS);
    sendUserData (FRSKY_GPS_LONG_B, eLeReS.LonB);
    sendUserData (FRSKY_GPS_LONG_A, eLeReS.LonA);
    sendUserData (FRSKY_GPS_LAT_B, eLeReS.LatB);
    sendUserData (FRSKY_GPS_LAT_A, eLeReS.LatA);
    //sendUserData (FRSKY_RPM, 11111 / 60);
    sendUserData (FRSKY_FUEL, eLeReS.FUEL);
  }
}

int ObliczFuel ()
{
  float fuel;
  char text[50];
  float BAT_MIN = 85;
  float BAT_MAX = 126;
  
  fuel= eLeReS.uRX - BAT_MIN;
  fuel = fuel * (100 / (BAT_MAX - BAT_MIN));
  if (fuel < 0) fuel=0;
  return fuel;
}

void sendUserData(uint8_t id, int16_t val)
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
      else                     FrskyData->write (d[i]);
    }

    FrskyData->write (0x5E);   // End of frame
    FrskyData->write (0x7E);   // End of frame

    if (FrskyData != NULL) port->flush();
  }
}

void sendLinkData(uint8_t A1, uint8_t A2, uint8_t Rssi, uint8_t Rcq)
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

    if (FrskyData != NULL) port->flush();
  }
}

void readLRS()
{
  String str;
  String tmp;
  String wynik;
  String nazwa;
  String wartosc;
  char text[50];
  float lat;
  float lon;

  RSSI_OK++;
  if (eLeReSData->available() > 0) {
    delay(1);
    blink1();
    str = eLeReSData->readStringUntil('\n');
    str.replace(", ", ",");

    for (uint8_t x = 0; x < 20; x++)
    {
      String xval = getValue(str, ' ', x); //wydzielenie pary parametr=wartosc
      if (xval != NULL)
      {

        nazwa = getValue(xval, '=', 0);
        wartosc = getValue(xval, '=', 1);

        if (nazwa == "RSSI") {
          eLeReS.RSSI = wartosc.toInt();
          RSSI_OK = 0;
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.RSSI:%d", eLeReS.RSSI);
          Serial.println(text);
#endif
        } else if (nazwa == "RCQ") {
          eLeReS.RCQ = wartosc.toInt();
        } else if (nazwa == "U") {
          eLeReS.uRX = atof (wartosc.c_str()) * 10;
          eLeReS.FUEL = ObliczFuel();
          if (eLeReS.uRX > 124) eLeReS.uRX = 124; //błąd wyliczania namięcia powyżej 12,4v - do znalezienia
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.uRX:%d", eLeReS.uRX);
          Serial.println(text);
          sprintf(text, "rcv-eLeReS.FUEL:%d", eLeReS.FUEL);
          Serial.println(text);
#endif
        } else if (nazwa == "T") {
          eLeReS.tRX = wartosc.toInt();
          //eLeReSData->println(eLeReS.tRX);
        } else if (nazwa == "I") {
          eLeReS.aRX = atof (wartosc.c_str()) * 10;
        } else if (nazwa == "UTX") {
          eLeReS.uTX = atof (wartosc.c_str()) * 10;
        } else if (nazwa == "TTX") {
          eLeReS.tTX = wartosc.toInt();
          //eLeReSData->println(eLeReS.tTX);
        } else if (nazwa == "P") {
          eLeReS.P = wartosc.toInt();
        } else if (nazwa == "F") {
          eLeReS.TRYB = wartosc.toInt();
        } else if (nazwa == "HD") {
          eLeReS.HDop = atof (wartosc.c_str()) * 10; //?
        } else if (nazwa == "f") {
          eLeReS.FIX = wartosc.toInt();
        } else if (nazwa == "s") {
          eLeReS.SAT = wartosc.toInt();
        } else if (nazwa == "c") {
          eLeReS.KURS = wartosc.toInt();
        } else if (nazwa == "v") {
          eLeReS.v = wartosc.toInt();
        } else if (nazwa == "h") {
          eLeReS.h = wartosc.toInt();
        } else if (nazwa == "Pos") {
          tmp = getValue(wartosc, ',', 0); //pobranie lattitude
          lat = atof (tmp.c_str());
          eLeReS.LatB = (uint16_t)lat;
          lat = (lat - (float)eLeReS.LatB) * 60.0;
          eLeReS.LatB = eLeReS.LatB * 100 + (uint16_t)lat;
          eLeReS.LatA = (uint16_t)round((lat - (uint16_t)lat) * 10000.0);

          tmp = getValue(wartosc, ',', 1); //pobranie longtitude
          lon = atof (tmp.c_str());
          eLeReS.LonB = (uint16_t)lon;
          lon = (lon - (float)eLeReS.LonB) * 60.0;
          eLeReS.LonB = eLeReS.LonB * 100 + (uint16_t)lon;
          eLeReS.LonA = (uint16_t)round((lon - (uint16_t)lon) * 10000.0);
#ifdef DEBUG
          sprintf(text, "rcv-eLeReS.LatB:%d", eLeReS.LatB);
          Serial.println(text);
          sprintf(text, "rcv-eLeReS.LatA:%d", eLeReS.LatA);
          Serial.println(text);
          sprintf(text, "rcv-eLeReS.LonB:%d", eLeReS.LonB);
          Serial.println(text);
          sprintf(text, "rcv-eLeReS.LonA:%d", eLeReS.LonA);
          Serial.println(text);
#endif
        }
      }
    }
  }
  if (RSSI_OK > 500)
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
    eLeReS.HDop = NULL;
    eLeReS.FIX = NULL;
    eLeReS.SAT = NULL;
    eLeReS.KURS = NULL;
    eLeReS.v = NULL;
    eLeReS.h = NULL;
  }
}



void loop()
{
  delay (50);
  readLRS();
  sendAllData();
}


