

time_t prevDisplay = 0; // when the digital clock was displayed
/* This sample code demonstrates the normal use of a TinyGPS object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

TinyGPS gps;
#define ss Serial

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
 bool print_date(TinyGPS &gps);
static void print_str(const char *str, int len);

void init_gps()
{
  ss.begin(9600);
}

bool read_gps()
{
  
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;
 
  gps.f_get_position(&flat, &flon, &age);
  print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
  print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
  bool check = print_date(gps);
  
  smartdelay(1000);

  return check;
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void print_float(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    while (len-- > 1);
   //   Serial.print('*');
  //  Serial.print(' ');
  }
  else
  {
 //   Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i);
  //    Serial.print(' ');
  }
  smartdelay(0);
}

static void print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val == invalid)
   strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
 // Serial.print(sz);
   
  // lcd.clear ();
   lcd.setCursor ( 0, 1 );
   lcd.print (sz);
  smartdelay(0);
}

bool print_date(TinyGPS &gps)
{
  int Year;
  byte Month, Day, Hour, Minute, Second, hundredths;
  unsigned long age;
  gps.crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    {
   //  Serial.print("********** ******** ");
      return false;
    }
  else
  {
    char sz[32];
    setTime(Hour, Minute, Second, Day, Month, Year);
    adjustTime(19800);
    
     sprintf(sz, "%02d/%02d %02d:%02d ",day(), month(), hour(), minute());
    // Serial.print(sz);
    // lcd.clear ();
    lcd.setCursor (0,0);
    lcd.print (sz);
  }
  smartdelay(0);
  return true;
}

static void print_str(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
   // Serial.print(i<slen ? str[i] : ' ');
  smartdelay(0);
}


