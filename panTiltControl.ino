#include <Servo.h>

Servo panServo;
Servo tiltServo;
String inBuf;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(">");
  // attach servos
  panServo.attach(9, 1000, 2000);
  tiltServo.attach(10, 1000, 2000);

}


void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.available() == 0){}
  char c = Serial.read();
  if(c == '\n')
  {
    if(inBuf.length() > 1)
    {
      switch(inBuf[0])
      {
        case 'P':
        {
          String s = inBuf.substring(1);
          int v = s.toInt() * 2 + 90;
          if((v >= 0) && (v <= 180))
          {
            panServo.write(v);
          }
          Serial.println("P>");
        }
        break;
  
        case 'T':
        {
          String s = inBuf.substring(1);
          int v = s.toInt() * 2 + 90;
          if((v >= 0) && (v <= 180))
          {
            tiltServo.write(v);
          }  
          Serial.println("T>");
        }
        break;
        
        default:
        break;  
      }
    }
    inBuf = "";
  }
  else if(c > ' ')
  {
    inBuf += c;
  }
  

}
