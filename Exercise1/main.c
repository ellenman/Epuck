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

bool objectOnTheRight(){
  bool detectObjectOnTheRight = false;
  int prox0 = get_calibrated_prox(0);
  int prox1 = get_calibrated_prox(1);

  if(prox0 > 200 || prox1 > 200 ){
    char str[100];
    int str_length = sprintf(str, "Object on the Right  ---- Sensor0: %d, Sensor1: %d\n",
                                         prox0, prox1);
    e_send_uart1_char(str, str_length);
    detectObjectOnTheRight = true;

  }
  return detectObjectOnTheRight;

}

bool objectOnTheLeft(){
  bool detectObjectOnTheLeft = false;
  int prox6 = get_calibrated_prox(6);
  int prox7 = get_calibrated_prox(7);



  if(prox6 > 200 || prox7 > 200 ){
    detectObjectOnTheLeft = true;
    char str1[100];
    int str_length1 = sprintf(str1, "Object on the Left  ---- Sensor6: %d, Sensor7: %d\n",
                                         prox6, prox7);
    e_send_uart1_char(str1, str_length1);
  }

  return detectObjectOnTheLeft;

}

bool trapped(){
  bool robotTrapped = false;
  int prox6 = get_calibrated_prox(6);
  int prox1 = get_calibrated_prox(1);

  if(prox1 > 200 || prox6 > 200 ){
    robotTrapped = true;
    char str3[100];
    int str_length3 = sprintf(str3, "Robot is Stuck  ---- Sensor1: %d, Sensor6: %d\n",
                                         prox1, prox6);
    e_send_uart1_char(str3, str_length3);
  }

  return robotTrapped;

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
    int counter = 0;
    bool rotating = false;
    int randomCounter = 0;


    /* Infinite loop. */
    while (1) {

      int left_speed = MAX_SPEED;
      int right_speed = MAX_SPEED;

      if(randomCounter % 2 == 0){
        randomCounter ++ ;
        if(trapped()){
          if(counter < 10){
            left_speed = MAX_SPEED;
            right_speed = -MAX_SPEED;
            rotating = true;
            counter++;
          }
          else{
            left_speed = MAX_SPEED;
            right_speed = MAX_SPEED;
            rotating = false;
            counter = 0;
          }

        }
        else if(objectOnTheRight() && !rotating){
          left_speed = -MAX_SPEED;
          right_speed = MAX_SPEED;
        }
        else if(objectOnTheLeft() && !rotating){
          left_speed = MAX_SPEED;
          right_speed = -MAX_SPEED;
        }
        else{
          randomCounter ++ ;
          if(trapped()){
            if(counter < 10){
              left_speed = -MAX_SPEED;
              right_speed = MAX_SPEED;
              rotating = true;
              counter++;
            }
            else{
              left_speed = MAX_SPEED;
              right_speed = MAX_SPEED;
              rotating = false;
              counter = 0;
            }

          }
          else if(objectOnTheLeft() && !rotating){
            left_speed = MAX_SPEED;
            right_speed = -MAX_SPEED;
          }
          else if(objectOnTheRight() && !rotating){
            left_speed = -MAX_SPEED;
            right_speed = MAX_SPEED;
          }

        }
      }
      left_motor_set_speed(left_speed);
      right_motor_set_speed(right_speed);
    	//waits 1 second
      chThdSleepMilliseconds(100);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
