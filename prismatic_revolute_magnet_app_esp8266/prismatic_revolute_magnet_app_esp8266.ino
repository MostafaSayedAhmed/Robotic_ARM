

//////////////// WiFi Libraries ///////////
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h> // ESP8266 specific web server library
 
////////////// Servo Library ////////////// 
#include <Servo.h>

// Define pin connections for the motor control
#define in1Pin D5
#define in2Pin D6
#define enaPin D7

// Function to move the revolute (rotation) servo to target angle
void reachRevoluteTarget(int *r_pos, int r_angle);

// Function to move the prismatic (pitch) servo to target angle
void reachPrismaticTarget(int *p_pos, int p_angle);

// Function to stop both servos
void stopServos(int *r_pos, int *p_pos);

// Create servo objects for two joints
Servo r_servo;  // Servo for rotation (r)
Servo p_servo;  // Servo for pitch (p)

// Define initial and maximum positions for servos
#define P_START 10
#define P_END   50
#define R_START 0
#define R_END   50

// Initialize current positions of servos
int r_pos = R_START;    // Current position of the rotation servo
int p_pos = P_START;    // Current position of the pitch servo


///////////////// WiFi SetUp /////////////
// Set these to your desired credentials.
const char *ssid = "Arm";
const char *password = "12345678";
WiFiServer server(80);
                 
void setup() {
  // Attach servos to specific pins
  r_servo.attach(D2);  // Attach the rotation servo to pin D2 (GPIO4)
  p_servo.attach(D3);  // Attach the pitch servo to pin D3 (GPIO0)

  // Set all the motor control pins to outputs
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enaPin, OUTPUT);

  digitalWrite(enaPin, HIGH); // Full speed

  Serial.begin(115200);
  Serial.println("Hello, Mo3ed!");
  
  Serial.println();
  Serial.println("Configuring access point...");
 
  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
bb  server.begin();
  Serial.println("Server started");
}

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                                          // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                 // make a String to hold incoming data from the client
    while (client.connected()) { // loop while the client's connected
      if (client.available()) {                                     // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                                   // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {                                     // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {              // if you got anything else but a carriage return character,
          currentLine += c;                  // add it to the end of the currentLine
        }

        if (currentLine.endsWith("Pos/1")) {    
          // Move to position (50, 50)
          reachRevoluteTarget(&r_pos, 20);
          reachPrismaticTarget(&p_pos, 0);
          // delay(1000); // Wait for 1 second
        }

        if (currentLine.endsWith("Col/1")) {
          // Collect with the DC magnet
          activateMagnet();
          // delay(2000); // Run the motor forward for 2 seconds
          //deactivateMagnet();
          //delay(1000); // Stop for 1 second
        }

        if (currentLine.endsWith("Home/1")) {
          // Move to position (0, 0)
          reachRevoluteTarget(&r_pos, 90);
          reachPrismaticTarget(&p_pos, 40);
          // delay(1000); // Wait for 1 second
        }

        if (currentLine.endsWith("Col/0")) {
          // Release with the DC magnet
          //activateMagnetReverse();
          //delay(2000); // Run the motor backward for 2 seconds
          deactivateMagnet();
          // delay(1000); // Stop for 1 second
        }

        if (currentLine.endsWith("Pos/0")) {
          // Move to position (-50, 50)
          reachRevoluteTarget(&r_pos, 120);
          reachPrismaticTarget(&p_pos, 0);
          // delay(1000); // Wait for 1 second
        }
      }
    }
    client.stop();                                  // close the connection:
    Serial.println("Client Disconnected.");
  }
}

// Function to move the revolute (rotation) servo to the target angle
void reachRevoluteTarget(int *r_pos, int r_angle) {
  if (*r_pos > r_angle) {
    // If the current position is greater than the target, decrement the position
    for (int i = *r_pos; i > r_angle; i--) {
      r_servo.write(i); // Move the servo to the current position
      delay(15); // Short delay for smooth movement
    }
  } else if (*r_pos < r_angle) {
    // If the current position is less than the target, increment the position
    for (int i = *r_pos; i < r_angle; i++) {
      r_servo.write(i); // Move the servo to the current position
      delay(15); // Short delay for smooth movement
    }
  }
  // Update the current position to the target position
  *r_pos = r_angle;
}

// Function to move the prismatic (pitch) servo to the target angle
void reachPrismaticTarget(int *p_pos, int p_angle) {
  if (*p_pos > p_angle) {
    // If the current position is greater than the target, decrement the position
    for (int i = *p_pos; i > p_angle; i--) {
      p_servo.write(i); // Move the servo to the current position
      delay(15); // Short delay for smooth movement
    }
  } else if (*p_pos < p_angle) {
    // If the current position is less than the target, increment the position
    for (int i = *p_pos; i < p_angle; i++) {
      p_servo.write(i); // Move the servo to the current position
      delay(15); // Short delay for smooth movement
    }
  }
  // Update the current position to the target position
  *p_pos = p_angle;
}

// Function to stop both servos
void stopServos(int *r_pos, int *p_pos) {
  r_servo.write(*r_pos); // Stop the rotation servo at the current position
  p_servo.write(*p_pos); // Stop the pitch servo at the current position
}

// Function to activate the DC magnet (run the motor forward)
void activateMagnet() {
  digitalWrite(in1Pin, HIGH);
  digitalWrite(in2Pin, LOW);
}

// Function to deactivate the DC magnet (stop the motor)
void deactivateMagnet() {
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, LOW);
}

// Function to activate the DC magnet in reverse (run the motor backward)
void activateMagnetReverse() {
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, HIGH);
}


//  //////////////// WiFi Librarires ///////////
// #include <WiFi.h>
// #include <WiFiClient.h>
// #include <WiFiAP.h>

 
// ////////////// Servo Library ////////////// 
// #include <Servo.h>

// // Define pin connections for the motor control
// #define in1Pin D5
// #define in2Pin D6
// #define enaPin D7

// // Function to move the revolute (rotation) servo to target angle
// void reachRevoluteTarget(int *r_pos, int r_angle);

// // Function to move the prismatic (pitch) servo to target angle
// void reachPrismaticTarget(int *p_pos, int p_angle);

// // Function to stop both servos
// void stopServos(int *r_pos, int *p_pos);

// // Create servo objects for two joints
// Servo r_servo;  // Servo for rotation (r)
// Servo p_servo;  // Servo for pitch (p)

// // Define initial and maximum positions for servos
// #define P_START 0
// #define P_END   50
// #define R_START 0
// #define R_END   50

// // Initialize current positions of servos
// int r_pos = R_START;    // Current position of the rotation servo
// int p_pos = P_START;    // Current position of the pitch servo


// ///////////////// WiFi SetUp /////////////
// // Set these to your desired credentials.
// const char *ssid = "Arm";
// const char *password = "12345678";
// WiFiServer server(80);

// void setup() {
//   // Attach servos to specific pins
//   r_servo.attach(4);  // Attach the rotation servo to pin 4
//   p_servo.attach(5);  // Attach the pitch servo to pin 5

//   // Set all the motor control pins to outputs
//   pinMode(in1Pin, OUTPUT);
//   pinMode(in2Pin, OUTPUT);
//   pinMode(enaPin, OUTPUT);

//   digitalWrite(enaPin, HIGH); // Full speed



//   Serial.begin(115200);
//   Serial.println("Hello, Mo3ed!");
  
//   Serial.println();
//   Serial.println("Configuring access point...");
 


//   // You can remove the password parameter if you want the AP to be open.
//   WiFi.softAP(ssid, password);
//   IPAddress myIP = WiFi.softAPIP();
//   Serial.print("AP IP address: ");
//   Serial.println(myIP);
//   server.begin();
//   Serial.println("Server started");
  
// }

// void loop() {

//   WiFiClient client = server.available();   // listen for incoming clients

//   if (client) 
//   {                                          // if you get a client,
//     Serial.println("New Client.");           // print a message out the serial port
//     String currentLine = "";                 // make a String to hold incoming data from the client
//     while (client.connected()) 
//     { // loop while the client's connected
//       if (client.available())
//       {                                     // if there's bytes to read from the client,
//         char c = client.read();             // read a byte, then
//         Serial.write(c);                    // print it out the serial monitor
//         if (c == '\n') 
//         {                                   // if the byte is a newline character

//                                             // if the current line is blank, you got two newline characters in a row.
//                                             // that's the end of the client HTTP request, so send a response:
//           if (currentLine.length() == 0)
//           {
//                                            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
//                                            // and a content-type so the client knows what's coming, then a blank line:
//             client.println("HTTP/1.1 200 OK");
//             client.println("Content-type:text/html");
//             client.println();

//                                            // the content of the HTTP response follows the header:
//             client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
//             client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");

//                                            // The HTTP response ends with another blank line:
//             client.println();
//             // break out of the while loop:
//             break;
//           }
//           else 
//           {                                     // if you got a newline, then clear currentLine:
//             currentLine = "";
//           }
//         }
//         else if (c != '\r')
//           {                                     // if you got anything else but a carriage return character,
//              currentLine += c;                  // add it to the end of the currentLine
//           }

// if (currentLine.endsWith("Pos/targetCollect")) 
// {    
//   // Move to position (50, 50)
//   reachRevoluteTarget(&r_pos, 0);
//   reachPrismaticTarget(&p_pos, 0);
//   delay(1000); // Wait for 1 second
// }

// if (currentLine.endsWith("Pos/collect"))
// {
//   // Collect with the DC magnet
//   activateMagnet();
//   delay(2000); // Run the motor forward for 2 seconds
//   //deactivateMagnet();
//   //delay(1000); // Stop for 1 second
// }


// if (currentLine.endsWith("Pos/targetRelease"))
// {
//   // Move to position (0, 0)
//   reachRevoluteTarget(&r_pos, 20);
//   reachPrismaticTarget(&p_pos, 20);
//   delay(1000); // Wait for 1 second
// }

// if (currentLine.endsWith("Pos/release"))
// {
//   // Release with the DC magnet
//   //activateMagnetReverse();
//   //delay(2000); // Run the motor backward for 2 seconds
//   deactivateMagnet();
//   delay(1000); // Stop for 1 second
// }

// if (currentLine.endsWith("Pos/home"))
// {
//   // Move to position (-50, 50)
//   reachRevoluteTarget(&r_pos, 40);
//   reachPrismaticTarget(&p_pos, 0);
//   delay(1000); // Wait for 1 second
// }

// }
//     }
//      client.stop();                                  // close the connection:
//     Serial.println("Client Disconnected.");
//   }
// }

// // Function to move the revolute (rotation) servo to the target angle
// void reachRevoluteTarget(int *r_pos, int r_angle) 
// {
//   if (*r_pos > r_angle) 
//   {
//     // If the current position is greater than the target, decrement the position
//     for (int i = *r_pos; i > r_angle; i--) 
//     {
//       r_servo.write(i); // Move the servo to the current position
//       delay(15); // Short delay for smooth movement
//     }
//   } else if (*r_pos < r_angle) 
//   {
//     // If the current position is less than the target, increment the position
//     for (int i = *r_pos; i < r_angle; i++) {
//       r_servo.write(i); // Move the servo to the current position
//       delay(15); // Short delay for smooth movement
//     }
//   }
//   // Update the current position to the target position
//   *r_pos = r_angle;
// }

// // Function to move the prismatic (pitch) servo to the target angle
// void reachPrismaticTarget(int *p_pos, int p_angle) 
// {
//   if (*p_pos > p_angle) 
//   {
//     // If the current position is greater than the target, decrement the position
//     for (int i = *p_pos; i > p_angle; i--) 
//     {
//       p_servo.write(i); // Move the servo to the current position
//       delay(15); // Short delay for smooth movement
//     }
//   } else if (*p_pos < p_angle) 
//   {
//     // If the current position is less than the target, increment the position
//     for (int i = *p_pos; i < p_angle; i++) {
//       p_servo.write(i); // Move the servo to the current position
//       delay(15); // Short delay for smooth movement
//     }
//   }
//   // Update the current position to the target position
//   *p_pos = p_angle;
// }

// // Function to stop both servos
// void stopServos(int *r_pos, int *p_pos) 
// {
//   r_servo.write(*r_pos); // Stop the rotation servo at the current position
//   p_servo.write(*p_pos); // Stop the pitch servo at the current position
// }

// // Function to activate the DC magnet (run the motor forward)
// void activateMagnet() 
// {
//   digitalWrite(in1Pin, HIGH);
//   digitalWrite(in2Pin, LOW);
// }

// // Function to deactivate the DC magnet (stop the motor)
// void deactivateMagnet() 
// {
//   digitalWrite(in1Pin, LOW);
//   digitalWrite(in2Pin, LOW);
// }

// // Function to activate the DC magnet in reverse (run the motor backward)
// void activateMagnetReverse() 
// {
//   digitalWrite(in1Pin, LOW);
//   digitalWrite(in2Pin, HIGH);
// }
