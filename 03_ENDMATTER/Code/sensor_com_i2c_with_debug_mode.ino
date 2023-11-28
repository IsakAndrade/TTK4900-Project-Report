/*
This code is written to gather sensor data from 3 IR sensors and 3 Super Sonic Sensors (HC-SR04).
*/

// PROGRAM CONSTANTS
bool debug_mode = true;
int poll_rate_ms = 100; 

// NOTICE: A timeout should be added!
// SUPER SONIC SENSOR
int NUM_SUPER_SONIC_SENSORS = 3;
const int trigPins[] = {2,4,6};
const int echoPins[] = {3,5,7};
float sonicSensorVals[3];

// IR SENSORS
const int NUM_IR_SENSORS = 3;
const int irSensorPins[] = {8,9,10}; 
int irSensorVals[3];

// COMUNICATION

// Data to received
struct I2cTxStruct {
    int irSensorVals[3];      //  6 [Bytes]
    float sonicSensorVals[3]; // 12 [Bytes]
    byte padding[14];         // 14 [Bytes]
};

// Data to sent
struct I2cRxStruct {
    char  command_type[16];   // 16 [Bytes]
    int command_param;        //  2 [Bytes]
    byte padding[14];         // 14 [Bytes]
};

I2cTxStruct txData = {{0,0,0}, {0.0,0.0,0.0}, 0};
I2cRxStruct rxData;

bool newTxData = false;
bool newRxData = false;
bool rqSent = false;

// these need to be swapped for the other Arduino
const byte thisAddress = 8; 
const byte otherAddress = 9;

// Timing variables
unsigned long prevUpdateTime = 0;
unsigned long updateInterval = 500;


// I2C Arduino library
#include <Wire.h>


void setup() {

  Serial.begin(9600);
  Serial.println("Serial Working"); // Test to check if serial is working or not
  
  // IR-SENSORS
  for (int i = 0; i <= NUM_IR_SENSORS; i++) {
    pinMode(irSensorPins[i], INPUT); // IR Sensor pin INPUT
  }

  // SUPER-SONIC SENSORS
  for (int i = 0; i <= NUM_SUPER_SONIC_SENSORS; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT); // IR Sensor pin INPUT
  }
  
  
  // set up I2C
  Wire.begin(thisAddress); // join i2c bus

  Wire.onReceive(receiveEvent); // register function to be called when a message arrives
  Wire.onRequest(requestEvent); // register function to be called when a request arrives


}
void loop() {
  // this bit checks if a message has been received
  if (debug_mode) {
    if (newRxData == true) {
        showNewData();
        newRxData = false;
    }
  }

  // this function updates the data in txData
  ir_sensor_measurement();
  sonic_sensor_measurements();

  updateDataToSend();
  // this function sends the data if one is ready to be sent
 
}


void updateDataToSend() {
   // update the data after the previous message has been
  if (rqSent == true) {
    rqSent = false;

    // Insert the sensor measurements
    for (int i = 0; i < NUM_IR_SENSORS; i++) {
      txData.irSensorVals[i] = irSensorVals[i];
    }

    for (int i = 0; i < NUM_SUPER_SONIC_SENSORS; i++) {
      txData.sonicSensorVals[i] = sonicSensorVals[i];
    }

  }
}

void showTxData() {
  // For demo show the data that as been sent
  Serial.println("\n Sent Data ");
  Serial.print("IR : ");
  for (int i = 0; i < NUM_IR_SENSORS; i++) {
    Serial.print(' ');
    Serial.print('|');
    Serial.print(' ');
    Serial.print(txData.irSensorVals[i]);    
  }

  Serial.print("\n");
 
  Serial.print("Super Sonic Sensors: ");
  for (int i = 0; i < NUM_SUPER_SONIC_SENSORS; i++) {
    Serial.print(' ');
    Serial.print('|');
    Serial.print(' ');
    Serial.print(txData.sonicSensorVals[i]);
  }
}

void showNewData() {
    Serial.print("This just in ");
    Serial.print(rxData.command_type);
    Serial.print(' ');
    Serial.println(rxData.command_param);
    
}

// This function is called by the Wire library when a message is received
void receiveEvent(int numBytesReceived) {

  if (newRxData == false) {
      // copy the data to rxData
      Wire.readBytes( (byte*) &rxData, numBytesReceived);
      newRxData = true;
  }
  else {
      // dump the data
      while(Wire.available() > 0) {
          byte c = Wire.read();
      }
  }
}


void requestEvent() {
    Wire.write((byte*) &txData, sizeof(txData));
    if (debug_mode) {
      showTxData();
    }
    
    rqSent = true;
}


// IR SENSORS
void ir_sensor_measurement() {
  for (int i = 0; i < NUM_IR_SENSORS; i++) {
    irSensorVals[i] = digitalRead(irSensorPins[i]);
  }
}

// SUPER SONIC SENSORS
void sonic_sensor_measurements() { 

  float duration, distance;

  for (int i = 0; i < NUM_SUPER_SONIC_SENSORS; i++) {
      
    // POLL SUPER SONIC SENSOR
    digitalWrite(trigPins[i], LOW);
    delayMicroseconds(2);
    digitalWrite(trigPins[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPins[i], LOW);

    duration = pulseIn(echoPins[i], HIGH);
    distance = (duration*.0343)/2;
    
    sonicSensorVals[i] = distance;
  }
}



