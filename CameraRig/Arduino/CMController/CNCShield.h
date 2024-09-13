/* 
 * Lightweight library for the Arduino CNC Shield:
 * https://blog.protoneer.co.nz/arduino-cnc-shield
 *
 * Written by Victor Gabriel Costin.
 * Licensed under the MIT license.
 */

#ifndef __CNCSHIELD_H__
#define __CNCSHIELD_H__
#include "Arduino.h"
typedef enum {
  COUNTER = 0,
  CLOCKWISE
} direction_t;


#define CLOCK_TICK (10)
class CNCShield;

class StepperMotor {
  enum {
    IDLE = 0,
    HOME,
    HOME_BACK_OFF,
    MOVE_CLOCKWISE,
    MOVE_COUNTER
  };
private:
  char id = '*';
  int state = IDLE;
  direction_t dir = COUNTER;
  int speed = 0;                /* steps per second */
  int delay_between_steps = 10; /* ms */
  int stp_pin;
  int dir_pin;
  int end_pin;
  bool was_init = false;
  //
  long current = 0;      // current position
  long target = 0;       // target position
  int delay_count = 0;  // count down timer for step interval
  bool invertDir = false;
  long next = 0;
  bool nextFlag = false; 
public:
  StepperMotor(int _stp_pin = 0, int _dir_pin = 0, int _end_pin = 0);
  long get_next() { return next;}
  void set_next(int v) { next = v; nextFlag = true;}
  void set_id(char i) { id = i;}

  void clear() { current = target = next = 0; delay_count = 0; nextFlag = false;}
  void init();
  bool is_init();

  int get_stp_pin();
  void set_stp_pin(int _stp_pin);

  int get_dir_pin();
  void set_dir_pin(int _dir_pin);

  int get_end_pin();
  void set_end_pin(int _end_pin);

  void _step();

public:
  bool step();
  bool step(direction_t _dir);
  bool step(int no_of_steps);
  bool step(int no_of_steps, direction_t _dir);
  void clearStep() { digitalWrite(stp_pin, LOW);}
  direction_t get_dir();
  void set_dir(direction_t _dir);

  double get_speed();
  void set_speed(int _speed);

  bool end_stop();

  long get_current() {
    return current;
  }  // current position
  long get_target() {
    return target;
  }  // target position
  void set_current(long v) {
    current = v;
  }
  void set_target(long v) {
    target = v;
  }
  void home() {
    state = HOME;
  }  // go home  until endstop
  void idle() {
    state = IDLE;
  }
  //
  bool  isIdle()  { return state == IDLE;}
  //
  bool   isActive() { return state != IDLE;}
  //
  void move() {
    if (current != target) {
      if (current < target) {
        moveClockwise();

      } else {
        moveCounter();
      }
    }
  }
  void moveClockwise() {
    state = MOVE_CLOCKWISE;
    delay_count = delay_between_steps;
  }
  void moveCounter() {
    state = MOVE_COUNTER;
    delay_count = delay_between_steps;
  }
  void set_targetDelta(long i) {
    set_target(get_target() + i);
  }  // move relative
  bool get_invertDir() {
    return invertDir;
  }
  void set_invertDir(bool f) {
    invertDir = f;
  }
  void set_State(int i) {
    state = i;
  }
  int get_State() {
    return state;
  }
  void drive();  // drive the state machine

  friend class CNCShield;
};

enum {
  MOTOR_X = 0,
  MOTOR_Y,
  MOTOR_Z
};
class CNCShield {
private:
  int enable_pin;
  StepperMotor motors[3];

public:
  CNCShield();

  void begin();
  void enable();
  void disable();

  StepperMotor* get_motor(unsigned int motor_id);
};

#endif /* __CNCSHIELD_H__ */
