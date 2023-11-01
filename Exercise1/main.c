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
#include "sensors/VL53L0X/VL53L0X.h"

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

// Define the MAX_SPEED constant with a value of 300
#define MAX_SPEED 500;

//detetct using proximity sensor 1 and 2 if there is an object on the right side
bool objectOnTheRight(){
  bool detectObjectOnTheRight = false;
  int prox1 = get_calibrated_prox(1);
  int prox2 = get_calibrated_prox(2);



if(prox1 > 200 || prox2 > 200 ){
    char str[100];
    int str_length = sprintf(str, "Object on the Right  ---- Sensor1: %d, Sensor2: %d\n",
                                         prox1, prox2);
    e_send_uart1_char(str, str_length);
    detectObjectOnTheRight = true;
  }

  return detectObjectOnTheRight;

}

//detetct using proximity sensor 5 and 6 if there is an object on the left side
bool objectOnTheLeft(){
  bool detectObjectOnTheLeft = false;
  int prox5 = get_calibrated_prox(5);
  int prox6 = get_calibrated_prox(6);

  if(prox5 > 200 || prox6 > 200 ){
    char str1[100];
    int str_length1 = sprintf(str1, "Object on the Left  ---- Sensor5: %d, Sensor6: %d\n",
                                         prox5, prox6);
    e_send_uart1_char(str1, str_length1);
    detectObjectOnTheLeft = true;
  }

  return detectObjectOnTheLeft;

}

//detetct using proximity sensor 3 if there is an object on the back right side
bool objectOnTheBackRight(){
  bool detectObjectOnTheBackRight = false;
  int prox3 = get_calibrated_prox(3);


  if(prox3 > 200 ){
    char str3[100];
    int str_length3 = sprintf(str3, "Object on the back Right  ---- Sensor3: %d",
                                         prox3);
    e_send_uart1_char(str3, str_length3);
    detectObjectOnTheBackRight = true;
  }

  return detectObjectOnTheBackRight;
}

//detetct using proximity sensor 4 if there is an object on the back left side
bool objectOnTheBackLeft(){
  bool detectObjectOnTheBackLeft = false;
  int prox4 = get_calibrated_prox(4);

  if(prox4 > 200 ){
    char str4[100];
    int str_length4 = sprintf(str4, "Object on the back left  ---- Sensor4: %d",
                                         prox4);
    e_send_uart1_char(str4, str_length4);
    detectObjectOnTheBackLeft = true;
  }

  return detectObjectOnTheBackLeft;
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
    VL53L0X_start();


    while (1) {

      int left_speed = MAX_SPEED;
      int right_speed = MAX_SPEED;
      //Get value from distance sensor
      uint16_t distance_mm = VL53L0X_get_dist_mm();
      double speed_increase_factor = 1.5;


      if(distance_mm > 10 && distance_mm < 20){
        left_speed = 0;
        right_speed = 0;
      }
      else if(distance_mm < 10){
        left_speed = -MAX_SPEED;
        right_speed = -MAX_SPEED ;
      }

      else if(objectOnTheRight()){
        left_speed = MAX_SPEED;
        right_speed = -MAX_SPEED;
      }
      else if(objectOnTheLeft()){
        left_speed = -MAX_SPEED;
        right_speed = MAX_SPEED;
      }
      else if(objectOnTheBackRight()){
        left_speed = MAX_SPEED;
        right_speed = -MAX_SPEED;
      }
      else if(objectOnTheBackLeft()){
        left_speed = -MAX_SPEED;
        right_speed = MAX_SPEED;
      }
      else if(distance_mm > 100){
        left_speed = MAX_SPEED * speed_increase_factor;
        right_speed = MAX_SPEED * speed_increase_factor;
      }
      else{
        left_speed = MAX_SPEED;
        right_speed = MAX_SPEED;
      }

      left_motor_set_speed(left_speed);
      right_motor_set_speed(right_speed);

      chThdSleepMilliseconds(100);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
