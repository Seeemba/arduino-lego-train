/*
 * Copyright (c) 2016 Intel Corporation.  All rights reserved.
 * See the bottom of this file for the license terms.
 */

#include <CurieBLE.h>
#include <legopowerfunctions.h>
#include <Servo.h>

BLEService trainService("19B10000-E8F2-537E-4F6C-D104768A1214"); // LEGO Train Service

// BLE Characteristic - custom 128-bit UUID, read and writable by central
BLEUnsignedCharCharacteristic trainCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

//set up the IR Port for the Arduino
LEGOPowerFunctions lego(3);
int trainPin = 13;                 // LED connected to digital pin 13
int currentSpeed = 1;
bool reverseFlag = false;

const int switchPin = 10; // pin to use for the LED
Servo myservo;

void setup() {
  Serial.begin(9600);

  // set LED pin to output mode
  pinMode(trainPin, OUTPUT);
//  pinMode(switchPin, OUTPUT);
  myservo.attach(9);

  // begin initialization
  BLE.begin();

  // set advertised local name and service UUID:
  BLE.setLocalName("LEGO-Train");
  BLE.setAdvertisedService(trainService);

  // add the characteristic to the service
  trainService.addCharacteristic(trainCharacteristic);

  // add service
  BLE.addService(trainService);

  // set the initial value for the characeristic:
  trainCharacteristic.setValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
}

void stopTrain() {
  Serial.println(F("Stop Train"));
  lego.SingleOutput(0, PWM_FLT, BLUE, CH1);
  digitalWrite(trainPin, LOW);    // sets the LED off
  currentSpeed = 1;
  delay(500);  
}

void moveTrain() {
  Serial.println(currentSpeed);
  lego.SingleOutput(0, currentSpeed, BLUE, CH1);
  digitalWrite(trainPin, HIGH);   // sets the LED on
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
        if (trainCharacteristic.written()) {
        // if the remote device wrote to the characteristic,
        switch (trainCharacteristic.value()) {
            case 0:
              stopTrain();
              break;          
            case 1:
              if (reverseFlag) {
                stopTrain();                
                reverseFlag = false;
              }
              Serial.println("Start Train");
              currentSpeed = 1;  
              moveTrain();
              break;
            case 2:
              stopTrain();
              Serial.println("Reverse Train");
              if (reverseFlag){
                reverseFlag = false;
                currentSpeed = 1;                
              } else {
                reverseFlag = true;
                currentSpeed = 15;
              }
              moveTrain();
              break;              
            case 3:
              Serial.println("Increase speed");
              if (currentSpeed < 7) {
                currentSpeed++;
              } else if (currentSpeed > 9) { // Reverse mode
                currentSpeed--;
              }
              moveTrain();
              break;
            case 4:
              Serial.println("Decrease speed");
              if(currentSpeed > 1 && currentSpeed < 8){
                currentSpeed--;
              } else if (currentSpeed > 8 && currentSpeed < 15) { // Reverse mode
                currentSpeed++;
              }
              moveTrain();
              break;                                        
            case 10:
              Serial.println(F("Switch Left"));
              myservo.write(60);
              delay(500);
              break;
            case 11:
              Serial.println(F("Switch Right"));
              myservo.write(0);
              delay(500);
              break;                        
            default: 
              Serial.println(F("Default"));
            break;
          }
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
}

/*
   Copyright (c) 2016 Intel Corporation.  All rights reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
