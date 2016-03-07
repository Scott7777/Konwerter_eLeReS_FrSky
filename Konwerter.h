
#define FRSKY_GPS_ALT         0x01
#define FRSKY_TEMP1         0x02
#define FRSKY_RPM           0x03
#define FRSKY_FUEL          0x04
#define FRSKY_TEMP2         0x05
#define FRSKY_ALT_B         0x10

#define FRSKY_GPS_SPEED_B   0x11
#define FRSKY_GPS_SPEED_A   0x19

#define FRSKY_GPS_LONG_B    0x12
#define FRSKY_GPS_LONG_A    0x1A
#define FRSKY_GPS_LONG_EW   0x22

#define FRSKY_GPS_LAT_B     0x13
#define FRSKY_GPS_LAT_A     0x1B
#define FRSKY_GPS_LAT_NS    0x23

#define FRSKY_GPS_COURSE_B  0x14
#define FRSKY_GPS_COURSE_A  0x1C

void blink1();
String getValue(String data, char separator, int index);
int ObliczFuel();

struct eLeReS_data
{
  int RSSI;
  int RCQ;
  int uRX;
  int aRX;
  int tRX;
  int P;
  int TRYB;
  int uTX;
  int tTX;
  int HDop;
  int FIX;
  int SAT;
  int KURS;
  int v;
  int h;
  int16_t LatB;
  uint16_t LatA;
  int16_t LonB;
  uint16_t LonA;
  int FUEL;
};

//CH to wartosci 8 kanalow RC z nadajnika w HEX 00-FF, 80 srodek.
//P to cisnienie z czujnika baro.
//F to tryb lotu MuliWii.
//Deb to wartosci zmiennych debug z MuliWii.
//Pos to pozycja GPS
//f to FIX GPS 0=brak, 1=2D fix, 2=3D fix
//s to ilosc satelit
//c to kurs w stopniach
//v to predkosc w km/h
//h to wysokosc w metrach
//UTX to napiecie baterii nadajnika RC

