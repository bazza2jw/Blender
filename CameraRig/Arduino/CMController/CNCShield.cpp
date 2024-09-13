/* 
 * Lightweight library for the Arduino CNC Shield:
 * https://blog.protoneer.co.nz/arduino-cnc-shield
 *
 * Written by Victor Gabriel Costin.
 * Licensed under the MIT license.
 */

#include "CNCShield.h"


#define X_STP_PIN 2
#define Y_STP_PIN 3
#define Z_STP_PIN 4

#define X_DIR_PIN 5
#define Y_DIR_PIN 6
#define Z_DIR_PIN 7

#define X_END_STOP 9
#define Y_END_STOP 10
#define Z_END_STOP 11


#define SHIELD_EN_PIN 8

// #define DEBUG

CNCShield::CNCShield() {
  motors[0].set_stp_pin(X_STP_PIN);
  motors[0].set_dir_pin(X_DIR_PIN);
  motors[0].set_end_pin(X_END_STOP);

  motors[1].set_stp_pin(Y_STP_PIN);
  motors[1].set_dir_pin(Y_DIR_PIN);
  motors[1].set_end_pin(Y_END_STOP);

  motors[2].set_stp_pin(Z_STP_PIN);
  motors[2].set_dir_pin(Z_DIR_PIN);
  motors[2].set_end_pin(Z_END_STOP);


  enable_pin = SHIELD_EN_PIN;
}

void CNCShield::begin() {
  pinMode(enable_pin, OUTPUT);

  for (int i = 0; i < 3; i++) {
    motors[i].init();
  }

#ifdef DEBUG
  Serial.begin(115200);
#endif /* DEBUG */
}

void CNCShield::enable() {
  digitalWrite(SHIELD_EN_PIN, LOW);

#ifdef DEBUG
  Serial.println("CNCShield::enable()");
#endif /* DEBUG */
}

void CNCShield::disable() {
  digitalWrite(SHIELD_EN_PIN, HIGH);

#ifdef DEBUG
  Serial.println("CNCShield::disable()");
#endif /* DEBUG */
}

StepperMotor* CNCShield::get_motor(unsigned int motor_id) {
  if (motor_id > 2)
    return &motors[2];

  return &motors[motor_id];
}

StepperMotor::StepperMotor(int _stp_pin, int _dir_pin, int _end_pin)
  : stp_pin(_stp_pin), dir_pin(_dir_pin), end_pin(_end_pin) {}

void StepperMotor::init() {
  pinMode(stp_pin, OUTPUT);
  pinMode(dir_pin, OUTPUT);
  pinMode(end_pin, INPUT_PULLUP);


  was_init = true;
}

bool StepperMotor::is_init() {
  return was_init;
}

int StepperMotor::get_stp_pin() {
  return stp_pin;
}

void StepperMotor::set_stp_pin(int _stp_pin) {
  stp_pin = _stp_pin;
}

int StepperMotor::get_dir_pin() {
  return dir_pin;
}

void StepperMotor::set_dir_pin(int _dir_pin) {
  dir_pin = _dir_pin;
}

int StepperMotor::get_end_pin() {
  return end_pin;
}

void StepperMotor::set_end_pin(int _end_pin) {
  end_pin = _end_pin;
}

bool StepperMotor::end_stop() {
  if (digitalRead(end_pin) == LOW) {
    Serial.println("EndStop");
    return true;
  }
  return false;
}

void StepperMotor::_step() {
  digitalWrite(stp_pin, HIGH);  // arm

  //digitalWrite(stp_pin, LOW);
}

bool StepperMotor::step() {
  if (end_stop()) return false;
  if (!is_init()) {
#ifdef DEBUG
    Serial.println("StepperMotor::step(): not init yet.");
#endif /* DEBUG */
    return false;
  }
  _step();
  delay(1);
  digitalWrite(stp_pin, LOW);
  delay(10);


#ifdef DEBUG
  Serial.println("StepperMotor::step(): stepped.");
#endif /* DEBUG */

  return true;
}

bool StepperMotor::step(direction_t _dir) {
  if (_dir != dir)
    set_dir(_dir);

  return step();
}

bool StepperMotor::step(int no_of_steps) {
  if (!is_init())
    return false;

  if (speed == 0)
    return false;

#ifdef DEBUG
  Serial.println("StepperMotor::step_n()");
#endif /* DEBUG */

  for (int i = 0; (i < no_of_steps) && (!end_stop()); i++) {
    step();
  }

  return true;
}

bool StepperMotor::step(int no_of_steps, direction_t _dir) {
  if (_dir != dir)
    set_dir(_dir);

  return step(no_of_steps);
}

direction_t StepperMotor::get_dir() {
#ifdef DEBUG
  Serial.print("StepperMotor::get_dir(): dir: ");
  Serial.println(dir);
#endif /* DEBUG */

  return dir;
}

void StepperMotor::set_dir(direction_t _dir) {
  if (_dir != dir) {
#ifdef DEBUG
    Serial.print("StepperMotor::set_dir(): dir changed from ");
    Serial.print(dir);
    Serial.print(" to ");
    Serial.println(_dir);
#endif /* DEBUG */
    dir = _dir;

    digitalWrite(dir_pin, dir);
  }
}

double StepperMotor::get_speed() {
  return speed;
}

void StepperMotor::set_speed(int _speed) {
  speed = _speed;
  delay_between_steps = 200 / _speed;  // speed is steps per second
}

// home the axis

void StepperMotor::drive() {
  switch (state) {
    case IDLE:
      break;
    case HOME:
      if (!end_stop()) {
        step(invertDir ? COUNTER : CLOCKWISE);
      } else {
        state = HOME_BACK_OFF;
        set_dir(invertDir ? CLOCKWISE : COUNTER);
      }

      break;
    case HOME_BACK_OFF:
      {
        if (end_stop()) {
          _step();
        } else {
          state = IDLE;
          current = target = 0;
        }
      }
      break;
    case MOVE_CLOCKWISE:
      if (!end_stop()) {
        set_dir(invertDir ? CLOCKWISE : COUNTER);
        if (delay_count-- <= 0) {
          delay_count = delay_between_steps;
          if (current != target) {
            if (current++ < target) _step();
          } else {
            if (nextFlag) {

              nextFlag = false;
              target = next;
              Serial.print("@");
              Serial.println(id);
            }
            else
            {
              state = IDLE;
              Serial.print("!");
              Serial.println(id);
            }
          }
        }
      }
      break;
    case MOVE_COUNTER:
      set_dir(invertDir ? COUNTER : CLOCKWISE);
      if (!end_stop()) {
        if (delay_count-- <= 0) {
          if (current != target) {
            delay_count = delay_between_steps;
            if (current-- > target) _step();
          } else {
            if (nextFlag) {
              nextFlag = false;
              target = next;
              Serial.print("@");
              Serial.println(id);
            }
            else
            {
              Serial.print("!");
              Serial.println(id);
              state = IDLE;
            }
          }
        }
      } else {
        state = HOME_BACK_OFF;
      }
      break;
    default:
      break;
  }
}
