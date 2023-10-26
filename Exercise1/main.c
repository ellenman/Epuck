#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <main.h>

#include "epuck1x/uart/e_uart_char.h"
#include "stdio.h"
#include "serial_comm.h"

#include "sensors/proximity.h"

#include "motors.h"

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

// Define the MAX_SPEED constant with a value of 300
#define MAX_SPEED 300;


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
  right_motor_set_speed(MAX_SPEED);
}

void stop(){
  left_motor_set_speed(0);
  right_motor_set_speed(0);
}

void bodyLedOn(){
  set_body_led(1);
}

void bodyLedOff(){
  set_body_led(0);
}


bool objectOnTheRight(){
  bool detectObjectOnTheRight = false;
  int prox0 = get_calibrated_prox(0);
  int prox1 = get_calibrated_prox(1);

  char str[100];
  int str_length = sprintf(str, "Object on the Right  ---- Sensor0: %d, Sensor1: %d\n",
                                       proxValues[0], proxValues[1]);
  e_send_uart1_char(str, str_length);

  if(prox0 > 200 || prox1 > 200 ){
    detectObjectOnTheRight = true;
  }

  return detectObjectOnTheRight;

}

bool objectOnTheLeft(){
  bool detectObjectOnTheLeft = false;
  int prox6 = get_prox(6);
  int prox7 = get_prox(7);

  char str1[100];
  int str_length1 = sprintf(str1, "Object on the Left  ---- Sensor6: %d, Sensor7: %d\n",
                                       proxValues[6], proxValues[7]);
  e_send_uart1_char(str1, str_length1);

  if(prox6 > 200 || prox7 > 200 ){
    detectObjectOnTheLeft = true;
  }

  return detectObjectOnTheLeft;

}

int main(void)
{

    halInit();
    chSysInit();
    mpu_init();
    serial_start();
    messagebus_init(&bus, &bus_lock, &bus_condvar);
    proximity_start(0);
    calibrate_ir();
    motors_init();



    /* Infinite loop. */
    while (1) {



      if(objectOnTheRight()){
        turnLeft();
        bodyLedOn();
      }
      else if(objectOnTheLeft()){
        turnRight();
        bodyLedOn();
      }
      else{
        moveForward();
        bodyLedOff();
      }

    	//waits 1 second
      chThdSleepMilliseconds(1000);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
