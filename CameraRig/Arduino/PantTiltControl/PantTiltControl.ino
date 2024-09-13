#include <Servo.h>

#define TICK_TIME (100)
#define MIN_US (700)
#define MAX_US (2400)
#define RANGE_US (MAX_US - MIN_US)
class PTServo : public Servo {
  int _target;   // target angle 0 - 180
  int _current;  // current angle 0 - 180
  int _speed;    // degrees per second
  //
  // Degrees in pulsewidth
  int _current_us;
  int _target_us;
  int _speed_us;
  //
  unsigned long _startTime;
  //
public:
  PTServo()
    : _target(90), _current(90), _speed(10), _startTime(0) {
  }

  void setStartTime(unsigned long t) {
    _startTime = t;
  }
  int target() {
    return _target;
  }

  void setTarget(int t) {
    if ((t >= 0) && (t <= 180)) {
      _target = t;
      _target_us = map(t, 0, 180, MIN_US, MAX_US);
    }
  }

  bool onTarget() {
    return _target == _current;
  }

  int current() {
    return _current;
  }

  void setCurrent(int t) {
    if ((t >= 0) && (t <= 180)) {
      _current = t;
      _current_us = map(t, 0, 180, MIN_US, MAX_US);
    }
  }

  int speed() {
    return _speed;
  }

  void setSpeed(int s) {
    if ((s >= 0) && (s < 100)) {
      _speed = s;
      _speed_us = map(s, 0, 180, 0, RANGE_US);  // rate of change in pulse width
    }
  }

  void home() {
    noInterrupts();
    setTarget(90);
    setCurrent(90);
    write(90);
    interrupts();
  }

  void drive(unsigned long t);
};

void PTServo::drive(unsigned long t) {
  if (_current_us != _target_us) {
    if (t > _startTime) {
      unsigned long dt = t - _startTime;
      unsigned long d = (_speed_us * dt) / 1000L;  // distance travelled
      //
      if (d > 0) {
        if (_current_us < _target_us) {
          _current_us += (int)d;
          if (_current_us > _target_us) {
            _current_us = _target_us;
          }
        } else {
          _current_us -= (int)d;
          if (_current_us < _target_us) {
            _current_us = _target_us;
          }
        }
        _startTime = t;
        _current = map(_current_us,MIN_US,MAX_US,0,180);
        writeMicroseconds(_current_us);
      }
    } else {
      _startTime = t;
    }
  }
}


  // servo objects
  PTServo PanServo;
  PTServo TiltServo;
  bool tick = false;
  //

  struct CMDREC {
    int cmd;
    int p;   // pan
    int t;   // tilt
    int sp;  // speed pan
    int st;  // speed tilt
  };

  bool Run;
  int cmdPtr = 0;
  int cmdCount = 0;
  unsigned long waitTime = 0;
  bool waiting = false;

#define CMD_MAX (16)
  CMDREC cmds[CMD_MAX];

  enum {
    CMD_STOP = 0,
    CMD_GOTO,
    CMD_WAIT,  // wait a number of seconds
    CMD_LOOP
  };


  void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    PanServo.attach(10);
    TiltServo.attach(9);
    PanServo.home();
    TiltServo.home();
    PanServo.setSpeed(10);
    TiltServo.setSpeed(10);
  }

  void loop() {
    // put your main code here, to run repeatedly:
    if (Serial.available() > 0) {
      // read lines of CR/NL terminated text then parse it
      String s = Serial.readStringUntil('\n');
      s.trim();
      //


      switch (toUpperCase(s.charAt(0))) {

        case 'R':
          {
            Run = true;
            cmdPtr = 0;
            waiting = false;
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
            if (cmdCount < CMD_MAX) {
              CMDREC &c = cmds[cmdCount++];
              switch (toUpperCase(s.charAt(1))) {
                case 'G':
                  {
                    c.cmd = CMD_GOTO;
                    sscanf(s.c_str() + 2, "%d%d%d%d", &c.p, &c.t, &c.sp, &c.st);
                  }
                  break;
                case 'W':
                  {
                    c.cmd = CMD_WAIT;
                    sscanf(s.c_str() + 2, "%d", &c.t);
                  }
                  break;

                case 'X':
                  {
                    c.cmd = CMD_STOP;
                  }
                  break;

                case 'L':
                  {
                    c.cmd = CMD_LOOP;
                  }
                  break;

                default:
                  cmdCount--;  // step back / ignore
                  break;
              }
            }
          }
          break;


        case 'C':
          {
            cmdPtr = 0;
            cmdCount = 0;
            PanServo.setTarget(0);
            TiltServo.setTarget(0);
            PanServo.setSpeed(10);
            TiltServo.setSpeed(10);
          }
          break;

        case 'G':  // goto
          {
            int p = 0;
            int t = 0;
            int n = sscanf(s.c_str() + 1, "%d%d", &p, &t);
            if (n == 2) {
              noInterrupts();
              PanServo.setStartTime(millis());
              PanServo.setTarget(p);
              TiltServo.setTarget(t);
              TiltServo.setStartTime(millis());
              interrupts();
            }
            Serial.println(">");
          }
          break;

        case 'S':
          {
            int p = 0;
            int t = 0;
            if (sscanf(s.c_str() + 1, "%d%d", &p, &t) == 2) {
              noInterrupts();
              PanServo.setSpeed(p);
              TiltServo.setSpeed(t);
              interrupts();
            }
            Serial.println(">");
          }
          break;

        case '?':
          {
            char b[64];
            noInterrupts();
            sprintf(b, "> %d %d", PanServo.current(), TiltServo.current());
            interrupts();
            Serial.println(b);
          }
          break;
        case 'H':
          {
            // put to home (centre)
            PanServo.home();
            TiltServo.home();
          }
          break;
        case 'E':
          break;

        default:
          break;
      }
    }

    unsigned long t = millis();
    PanServo.drive(t);
    TiltServo.drive(t);
    delay(2);
    if (Run) {
      if (PanServo.onTarget() && TiltServo.onTarget())  // complete
      {
        if (waiting) {
          if (t > waitTime) {
            waiting = false;
          }
        } else {
          if (cmdCount > 0) {
            if (cmdPtr < cmdCount) {
              CMDREC &c = cmds[cmdPtr++];
              switch (c.cmd) {

                case CMD_STOP:
                  {
                    Run = false;
                    waiting = false;
                  }
                  break;
                case CMD_GOTO:
                  {
                    noInterrupts();
                    PanServo.setStartTime(t);
                    TiltServo.setStartTime(t);
                    PanServo.setTarget(c.p);
                    TiltServo.setTarget(c.t);
                    PanServo.setSpeed(c.sp);
                    TiltServo.setSpeed(c.st);
                    interrupts();
                  }
                  break;
                case CMD_WAIT:  // wait a number of seconds
                  {
                    waiting = true;
                    waitTime = t + c.t;
                  }
                  break;
                case CMD_LOOP:
                  {
                    cmdPtr = 0;
                  }
                  break;

                default:
                  Run = false;
                  cmdPtr = 0;
                  break;
              }
            } else {
              Run = false;
            }
          }
        }
      }
    }
  }
