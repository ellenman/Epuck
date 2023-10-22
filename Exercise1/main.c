#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <main.h>

#include "leds.h"
#include "spi_comm.h"

#include "motors.h"

#include "sensors/proximity.h"

#include "epuck1x/uart/e_uart_char.h"
#include "serial_comm.h">

// Define the MAX_SPEED constant with a value of 300
#define MAX_SPEED 300;
#define MAX_DISTANCE 500;

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

void bodyLedOn(){
  set_body_led(1);
}

void bodyLedOff(){
  set_body_led(0);
}


/*
void moveForward(){
  left_motor_set_speed(MAX_SPEED);
  right_motor_set_speed(MAX_SPEED);
}

void turnRight(){
  left_motor_set_speed(MAX_SPEED);
  right_motor_set_speed(-MAX_SPEED);
}

void turnLeft(){
  left_motor_set_speed(-MAX_SPEED);
  right_motor_set_speed(+MAX_SPEED);
}

void stop(){
  left_motor_set_speed(0);
  right_motor_set_speed(0);
}
*/

/*
Check the values of the proximity sensors might have to be 80
*/

bool objectOnTheRight(){
  bool detectObjectOnTheRight = false;
  int prox0 = get_prox(0);
  int prox1 = get_prox(1);
  int prox2 = get_prox(2);

  if(prox0 > 75 || prox1 > 75 || prox2 > 75 ){
    detectObjectOnTheRight = true;
  }

  return detectObjectOnTheRight;

}

bool objectOnTheLeft(){
  bool detectObjectOnTheLeft = false;
  int prox5 = get_prox(5);
  int prox6 = get_prox(6);
  int prox7 = get_prox(7);

  if(prox5 > 75 || prox6 > 75 || prox7 > 75 ){
    detectObjectOnTheLeft = true;
  }

  return detectObjectOnTheLeft;

}


int main(void)
{

    halInit();
    chSysInit();
    mpu_init();

    messagebus_init(&bus, &bus_lock, &bus_condvar);

    /* Infinite loop. */
    while (1) {

      char str[100];
      int messageForRight, messageForLeft, messageForward;
      messageForRight = sprintf(str, "Object found on the right side");
      messageForLeft = sprintf(str, "Object found on the left side");

    	//waits 1 second
      //chThdSleepMilliseconds(1000);
      proximity_start();
      serial_start();

      int left_speed = MAX_SPEED;
      int right_speed = MAX_SPEED;

      if(objectOnTheRight()){
        left_speed = MAX_SPEED;
        right_speed = -MAX_SPEED;
        e_send_uart1_char(str, messageForRight);
      }
      else if(objectOnTheLeft()){
        left_speed = -MAX_SPEED;
        right_speed = MAX_SPEED;
        e_send_uart1_char(str, messageForLeft);
      }
      left_motor_set_speed(left_speed);
      right_motor_set_speed(right_speed);

    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
