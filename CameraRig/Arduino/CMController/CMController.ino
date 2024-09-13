
#include "CNCShield.h"
#include "TimeInterrupt.h"


CNCShield cnc;  // the control

struct CMDREC {
  int cmd;  // command
  long x;    // arguments
  long y;
  long z;
};


int cmdPtr = 0;
bool Run = false;
int cmdCount = 0;
#define CMD_MAX (64)
CMDREC cmds[CMD_MAX];
unsigned long endTime = 0;
unsigned long startTime = 0;
bool waiting = false;

enum {
  CMD_STOP = 0,
  CMD_GOTO,
  CMD_SPEED,
  CMD_WAIT,
  CMD_LOOP
};




#define CMDLEN (64)
char cmd[CMDLEN];
int cmd_in = 0;
int state_wait_eol = false;  // waiting for EOL


bool drive() {
  cnc.get_motor(MOTOR_X)->drive();
  cnc.get_motor(MOTOR_Y)->drive();
  cnc.get_motor(MOTOR_Z)->drive();
  delay(2);
  cnc.get_motor(MOTOR_X)->clearStep();
  cnc.get_motor(MOTOR_Y)->clearStep();
  cnc.get_motor(MOTOR_Z)->clearStep();
  delay(1);
  return cnc.get_motor(MOTOR_X)->isIdle()
         && cnc.get_motor(MOTOR_Y)->isIdle()
         && cnc.get_motor(MOTOR_Z)->isIdle();  // true if all are idle = all have finished
}




void setup() {

  // put your setup code here, to run once:
  cnc.begin();
  Serial.begin(115200);
  //TimeInterrupt.begin();
  //TimeInterrupt.addInterrupt(TimerHandler, 20);
  //
  cnc.get_motor(MOTOR_X)->set_speed(10);
  cnc.get_motor(MOTOR_Y)->set_speed(200);
  cnc.get_motor(MOTOR_Z)->set_speed(10);
  cnc.get_motor(MOTOR_X)->set_invertDir(true);
  

  cnc.get_motor(MOTOR_X)->set_id('X');
  cnc.get_motor(MOTOR_Y)->set_id('Y');
  cnc.get_motor(MOTOR_Z)->set_id('Z');


  cnc.enable();

  memset(cmds, 0, sizeof(cmds));
}



bool fStep = false;

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial.available() > 0) {
    // read lines of CR/NL terminated text then parse it
    String s = Serial.readStringUntil('\n');
    s.trim();
    //
    switch (toUpperCase(s.charAt(0))) {

      case 'A':
        cnc.enable();
        Serial.println(">");
        break;

      case 'B':
        cnc.disable();
        Serial.println(">");
        break;

      case 'R':
        {
          // run the sequence
          Run = true;
          waiting = false;
          cmdPtr = 0;
        }
        break;
      case 'X':
        {
          Run = false;
          waiting = false;
        }
        break;

      case 'P':
        {
          if (cmdCount < (CMD_MAX - 1)) {
            CMDREC &c = cmds[cmdCount];
            // program
            switch (toUpperCase(s.charAt(1))) {
              case 'G':  // goto
                {
                  int n = sscanf(s.c_str() + 2, "%ld%ld%ld", &c.x, &c.y, &c.z);
                  if (n == 3) {
                    c.cmd = CMD_GOTO;
                    cmdCount++;
                    Serial.println(">");
                  } else {
                    Serial.println("?");
                  }
                }
                break;

              case 'S':  // speed
                {
                  int n = sscanf(s.c_str() + 2, "%ld%ld%ld", &c.x, &c.y, &c.z);
                  if (n == 3) {
                    c.cmd = CMD_SPEED;
                    cmdCount++;
                    Serial.println(">");
                  } else {
                    Serial.println("?");
                  }
                }
                break;

              case 'W':
                {
                  int n = sscanf(s.c_str() + 2, "%ld", &c.x);
                  if (n == 1) {
                    c.cmd = CMD_WAIT;
                    cmdCount++;
                    Serial.println(">");
                  } else {
                    Serial.println("?");
                  }
                }
                break;

              case 'L':  // loop - goto to zero
                {
                  c.cmd = CMD_LOOP;
                  cmdCount++;
                  Serial.println(">");
                }
                break;

              case 'X':
                {
                  c.cmd = CMD_STOP;
                  cmdCount++;

                  Serial.println(">");
                }
                break;

              default:
                Serial.println("?");
                break;
            }
          } else {
            Serial.println("? Full");
          }
          break;

          case 'N':  // goto
            {
              long x = 0;
              long y = 0;
              long z = 0;
              int n = sscanf(s.c_str() + 1, "%ld%ld%ld", &x, &y, &z);

              if (n > 0) {
                cnc.get_motor(MOTOR_X)->set_next(x);
                cnc.get_motor(MOTOR_Y)->set_next(y);
                cnc.get_motor(MOTOR_Z)->set_next(z);
              }
              Serial.println(">");
            }
            break;



          case 'G':  // goto
            {
              int x = 0;
              int y = 0;
              int z = 0;
              int n = sscanf(s.c_str() + 1, "%ld%ld%ld", &x, &y, &z);

              if (n == 3) {
                cnc.get_motor(MOTOR_X)->set_target(x);
                cnc.get_motor(MOTOR_X)->move();
                cnc.get_motor(MOTOR_Y)->set_target(y);
                cnc.get_motor(MOTOR_Y)->move();
                cnc.get_motor(MOTOR_Z)->set_target(z);
                cnc.get_motor(MOTOR_Z)->move();
                fStep = false;
              }
              Serial.println(">");
            }
            break;

          case 'S':
            {
              int x = 0;
              int y = 0;
              int z = 0;
              if (sscanf(s.c_str() + 1, "%d%d%d", &x, &y, &z) == 3) {
                cnc.get_motor(MOTOR_X)->set_speed(x);
                cnc.get_motor(MOTOR_Y)->set_speed(y);
                cnc.get_motor(MOTOR_Z)->set_speed(z);
              }
            }
            break;

          case '?':
            {
              char b[64];
              sprintf(b, "> %d %ld %d %ld %d %ld",
                      cnc.get_motor(MOTOR_X)->get_State(),
                      cnc.get_motor(MOTOR_X)->get_current(),
                      cnc.get_motor(MOTOR_Y)->get_State(),
                      cnc.get_motor(MOTOR_Y)->get_current(),
                      cnc.get_motor(MOTOR_Z)->get_State(),
                      cnc.get_motor(MOTOR_Z)->get_current());
              Serial.println(b);
            }
            break;
          case 'H':
            {
              cnc.get_motor(MOTOR_X)->home();
              cnc.get_motor(MOTOR_Y)->home();
              cnc.get_motor(MOTOR_Z)->home();
              Serial.println(">");
            }
            break;
          case 'E':
            cnc.get_motor(MOTOR_X)->idle();
            cnc.get_motor(MOTOR_Y)->idle();
            cnc.get_motor(MOTOR_Z)->idle();
            Serial.println(">");
            break;

          case 'C':  // clears positions - in effect forces home position and stops
            cnc.get_motor(MOTOR_X)->clear();
            cnc.get_motor(MOTOR_Y)->clear();
            cnc.get_motor(MOTOR_Z)->clear();
            //
            Run = false; // Clear the program
            waiting = false;
            cmdPtr = 0;
            cmdCount = 0;  // clear the buffer
            //
            Serial.println(">");
            break;

          default:
            Serial.println("?");
            break;
        }
    }
  }
  //
  //
  bool allIdle = cnc.get_motor(MOTOR_X)->isIdle() && cnc.get_motor(MOTOR_Y)->isIdle() && cnc.get_motor(MOTOR_Z)->isIdle();  // true if all are idle = all have finished
  bool anyActive = cnc.get_motor(MOTOR_X)->isActive() || cnc.get_motor(MOTOR_Y)->isActive() || cnc.get_motor(MOTOR_Z)->isActive();
  //
  if (anyActive) {
      cnc.get_motor(MOTOR_X)->drive();
      cnc.get_motor(MOTOR_Y)->drive();
      cnc.get_motor(MOTOR_Z)->drive();
      delayMicroseconds(10);
      cnc.get_motor(MOTOR_X)->clearStep();
      cnc.get_motor(MOTOR_Y)->clearStep();
      cnc.get_motor(MOTOR_Z)->clearStep();
      
    }



  if (allIdle) {
    // command interpreter
    if (Run) {
      if (waiting) {
        if (millis() > endTime) {
          waiting = false;
        }
      } else {
        if (cmdPtr < cmdCount) {
          CMDREC &c = cmds[cmdPtr++];
          switch (c.cmd) {
            case CMD_STOP:
              Run = false;
              break;
            case CMD_GOTO:
              {
                cnc.get_motor(MOTOR_X)->set_target(c.x);
                cnc.get_motor(MOTOR_X)->move();
                cnc.get_motor(MOTOR_Y)->set_target(c.y);
                cnc.get_motor(MOTOR_Y)->move();
                cnc.get_motor(MOTOR_Z)->set_target(c.z);
                cnc.get_motor(MOTOR_Z)->move();
                fStep = false;
              }
              break;
            case CMD_SPEED:
              {
                cnc.get_motor(MOTOR_X)->set_speed((int)c.x);
                cnc.get_motor(MOTOR_Y)->set_speed((int)c.y);
                cnc.get_motor(MOTOR_Z)->set_speed((int)c.z);
              }
              break;
            case CMD_WAIT:
              {
                endTime = millis() + ((unsigned long)c.x) * 1000;
                waiting = true;
              }
              break;
            case CMD_LOOP:
              {
                cmdPtr = 0;
              }
              break;

            default:
              Run = false;
              break;
          }
        } else {
          Run = false;  // end of sequence
        }
      }
    }
  }
}
