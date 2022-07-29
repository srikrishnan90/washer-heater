#include <Wire.h>

const int buzzer = 10; //buzzer to arduino pin 9
const int sen_plug = 13;
const int sen = 12;

int Incu_ThermistorPin = A0;
int Vo;
float plug = 0;
float sensor = 0;
float R1 = 10000;
float logR2, R2, IT, I_Tc;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

String de;
String te;

int dur = 0;
int pwm_on=250;
int pwm_off=0;

void setup()
{
  Wire.begin(0x08); //Set Arduino up as an I2C slave at address 0x07
  Wire.onRequest(requestEvent); //Prepare to send data
  Wire.onReceive(receiveEvent); //Prepare to recieve data
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
  pinMode(sen_plug, INPUT_PULLUP);
  pinMode(sen, INPUT_PULLUP);  
  Serial.begin(9600);
}

void loop()
{
  Wire.begin(0x08);
  delay(50);
  Vo = analogRead(Incu_ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  IT = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  I_Tc = IT - 273.15;
  plug=digitalRead(sen_plug);    
  sensor=digitalRead(sen);
    
  if (I_Tc < 37)
    analogWrite(9, pwm_on);
  else
    analogWrite(9, pwm_off);
  delay(50);
  if (te.substring(0, 3) == "buz")
  {
    buzer(dur);
    de = "done";
    te = "";
  }
}

void washing_start()
{
  pwm_on=100;
  pwm_off=100;
}

void washing_finish()
{
  pwm_on=250;
  pwm_off=0;
}

void buzer(int dur)
{
  uint32_t period = dur * 1000L;
  for ( uint32_t tStart = millis();  (millis() - tStart) < period;  )
  {
    tone(buzzer, 2000); // Send 1KHz sound signal...
    delay(100);        // ...for 1 sec
    noTone(buzzer);     // Stop sound...
    delay(100);        // ...for 1sec
  }
}


void requestEvent()
{
  String s1 = String(I_Tc, 1);
  String s2 = String(plug, 0);
  String s3 = String(sensor, 0);
  String all = s1 + " " + s2 + " " + s3;
  char buf[30];
  all.toCharArray(buf, 30);
  Wire.write(buf, 30);
}

void receiveEvent(int numBytes)
{
  char rc[30] = "";
  int count = 0;
  while (Wire.available())
  {
    char c = Wire.read();
    rc[count] = c;
    count++;
    delay(10);
  }
  de = rc;
  te = de;
  if (te.substring(0, 3) == "buz")
  {
    char buf[30];
    te.toCharArray(buf, 30);
    char *p = buf;
    char *str;
    int i = 0;
    while ((str = strtok_r(p, " ", &p)) != NULL) // delimiter is the space
    {
      if (i == 1)
      {
        dur = atoi(str);
      }
      i++;
    }
  }
  else if (te.substring(0, 3) == "was")
  {
    washing_start();
    de = "done";
    te = "";
  }
  else if (te.substring(0, 3) == "waf")
  {
    washing_finish();
    de = "done";
    te = "";
  }
}
