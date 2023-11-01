

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

#include "leds.h"
#include "spi_comm.h"
#include "selector.h"
#include "sensors/VL53L0X/VL53L0X.h"

#define MAX_SPEED 400

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

void stop(){
  left_motor_set_speed(0);
  right_motor_set_speed(0);
}

void moveForward(){
  left_motor_set_speed(MAX_SPEED);
  right_motor_set_speed(MAX_SPEED);
}

void goBackwards(){
  left_motor_set_speed(-MAX_SPEED);
  right_motor_set_speed(-MAX_SPEED);
}

void goFast(){
  left_motor_set_speed(800);
  right_motor_set_speed(800);
}



void rotateLeft(){
  left_motor_set_speed(-MAX_SPEED);
  right_motor_set_speed(MAX_SPEED);
}

void rotateRight(){
  left_motor_set_speed(MAX_SPEED);
  right_motor_set_speed(-MAX_SPEED);
}

void trappedMovement(){
	  rotateRight();
	  chThdSleepMilliseconds(500);
	  stop();
	  chThdSleepMilliseconds(500);
	  rotateRight();

}

bool objectOnTheRight(){
  bool detectObjectOnTheRight = false;
  int prox0 = get_calibrated_prox(0);
  int prox1 = get_calibrated_prox(1);

  if(prox0 > 450 || prox1 > 450 ){
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



  if(prox6 > 450 || prox7 > 450 ){
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

  if(prox1 > 250 && prox6 > 250 ){
    robotTrapped = true;
    char str3[100];
    int str_length3 = sprintf(str3, "Robot is Stuck  ---- Sensor1: %d, Sensor6: %d\n",
                                         prox1, prox6);
    e_send_uart1_char(str3, str_length3);
  }

  return robotTrapped;

}

//detetct using proximity sensor 1 and 2 if there is an object on the right side
bool objectOnTheRight2(){
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
bool objectOnTheLeft2(){
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
bool objectOnTheBackRight2(){
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
bool objectOnTheBackLeft2(){
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
    clear_leds();
    spi_comm_start();
    VL53L0X_start();


    /* Infinite loop. */
    while (1) {

        if(get_selector() == 1) {
          //Task1
        	set_body_led(1);
        	if(trapped()){
        		trappedMovement();
        	 }

        	 else if(objectOnTheRight()){
        		 set_body_led(0);
        		 rotateLeft();
        	  }

        	  else if(objectOnTheLeft()){
        	    	 set_body_led(0);
        	         rotateRight();
        	      }
        	      else{
        	        moveForward();
        	      }

        	    	//waits 1 second
        	      chThdSleepMilliseconds(100);
        } else if(get_selector() == 7) {
          //Task2
        	set_body_led(1);


        	      if(VL53L0X_get_dist_mm() >= 40 && VL53L0X_get_dist_mm() < 50){
        	        stop();
        	        set_body_led(0);
        	      }
        	      else if(VL53L0X_get_dist_mm() < 40){
        	        goBackwards();
        	        set_body_led(2);
        	      }

        	      else if(objectOnTheRight2()){
        	    	  rotateRight();
        	      }
        	      else if(objectOnTheLeft2()){
        	    	  rotateLeft();
        	      }
        	      else if(objectOnTheBackRight2()){
        	    	  rotateRight();
        	      }
        	      else if(objectOnTheBackLeft2()){
        	    	  rotateLeft();
        	      }
        	      else if(VL53L0X_get_dist_mm() > 50 && VL53L0X_get_dist_mm() < 300 ){
        	    	  goFast();
        	      }
        	      else if(VL53L0X_get_dist_mm() > 300  ){
        	           stop();
        	      }

        	      else{
          	        moveForward();

        	      }

        }
        else if(get_selector() == 3){
          left_motor_set_speed(0);
          right_motor_set_speed(0);
        }
        chThdSleepMilliseconds(100);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
