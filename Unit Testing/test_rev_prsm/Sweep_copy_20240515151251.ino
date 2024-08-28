/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep
*/

#include <Servo.h>

Servo r_servo;  // create servo object to control a servo
Servo p_servo;  // create servo object to control a servo

// twelve servo objects can be created on most boards
#define P_START 0
#define P_END   50
#define R_START 0
#define R_END   50

int r_pos = R_START;    // variable to store the servo r_position
int p_pos = P_START;
int flag_p = 1; //  0 if decrement 
int flag_r = 1; //  0 if decrement 


void setup() {
  r_servo.attach(5);  // attaches the servo on pin 8 to the servo object
  p_servo.attach(4);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  // for (r_pos = 0; r_pos <= 180; r_pos += 1) { // goes from 0 degrees to 180 degrees
  //   // in steps of 1 degree
  //   r_servo.write(r_pos);              // tell servo to go to r_position in variable 'r_pos'
  //   delay(15);                       // waits 15 ms for the servo to reach the r_position
  // }

  // for (r_pos = 180; r_pos >= 0; r_pos -= 1) { // goes from 180 degrees to 0 degrees
  //   r_servo.write(r_pos);              // tell servo to go to r_position in variable 'r_pos'
  //   delay(15);                       // waits 15 ms for the servo to reach the r_position
  // }

  p_servo.write(p_pos); 
  r_servo.write(r_pos); 
  
  if (flag_p== 1){
    p_pos += 1;
    delay(15);
    if (p_pos == P_END){      
      flag_p = 0;
    }
  }
  else if (flag_p== 0){
    p_pos -= 1;
    delay(15);
    if (p_pos == P_START){
      flag_p = 1;
    }
  } 

 // delay(15);                       // waits 15 ms for the servo to reach the r_position

  if (flag_r== 1){
    r_pos += 1;
    delay(15);
    if (r_pos == R_END){      
      flag_r = 0;
    }
  }  
  else if (flag_r== 0){
    r_pos -= 1;
    delay(15);
    if (r_pos == R_START){
      flag_r = 1;
    }
  }   

}
