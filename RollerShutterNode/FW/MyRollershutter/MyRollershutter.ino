/* *****************************************************************************************************************************
 * ROLLER SHUTTER NODE
 * Created by (2016) Pascal Moreau / Scalz
 * 
 * For Mysensors community :)
 * 
 * DESCRIPTION : Roller shutter node with autocalibration option, for Mysensors lib.
 * - opening 0-100%
 * -  types of calibration : 
 *      1) manual       : measure by yourself your up an down delays, and fill values in corresponding DEFINE. 
 *      2) semi-manual  : you can use a command from controller to simulate and endstop detection during autocalibration
 *      2) automatic    : use of ACS712 to detect if motor is running or not, if it has reached endstop.
 *      
 * - controlled by : Controller, Buttons, or I2C Interface
 * - OTA 
 * - and authentication features
 * 
 * PIN MAP
 * 
 * A7 : ADC acs712 (ACS712 5A Current detection sensor)
 * A1 : Up or Down Relay,  (SPDT 10A) * 
 * A2 : Motor Relay Power, ON/OFF motor(SPST-NO 10A)
 * A3 : ATSHA Authentication ic
 * D4 : DS18B20 temperature sensor (check onboard temperature, for instance to check Hi-Link AC-DC converter)
 * 
 * A4 : Up Button (ACTIVE LOW) 
 * A5 : Down Button (ACTIVE LOW)
 * D3:  Stop Button (ACTIVE LOW)
 * 
 * Libs used :  - Mysensors 2.0               
 *              - Bounce2
 *              - OneWire
 *              
 *              
 * ***************************************************************************************************************************** */ 

/* 
TODO LIST :
- check missing stuff for auth, ota
- if calibration in progress, no ota. If ota in progress, no calibration. other usecase
- accuracy of autocalibration...
- add a way to open/close all shutters from the buttons
- etc.
*/

#include "MyNodeDefinition.h"
#include "shutterSM.h"
#include <SPI.h>
#include <MySensors.h>

#include <Bounce2.h>
#include <OneWire.h>


// Define various ADC prescaler
const unsigned char PS_16   = (1 << ADPS2);
const unsigned char PS_32   = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64   = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128  = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

/* *****************************************************************************************************************************
 *  Globals
 * ***************************************************************************************************************************** */
// Temperature
float oldTemperature     = 0;
float hilinkTemperature  = 0;
OneWire  ow_ds1820(DS18B20_PIN);

// Authentication
const int sha204Pin = ATSHA204_PIN;
//atsha204Class sha204(sha204Pin);

// Buttons
Bounce debounceUp    = Bounce(); 
Bounce debounceDown  = Bounce();
Bounce debounceStop  = Bounce();

// MYSENSORS
MyMessage msgUp(CHILD_ID_ROLLERSHUTTER,V_UP);                         // Message for Up 
MyMessage msgDown(CHILD_ID_ROLLERSHUTTER,V_DOWN);                     // Message for Down 
MyMessage msgStop(CHILD_ID_ROLLERSHUTTER,V_STOP);                     // Message for Stop 
MyMessage msgShutterPosition(CHILD_ID_ROLLERSHUTTER,V_PERCENTAGE);    // Message for % shutter 
MyMessage msgTemperature(CHILD_ID_TEMPERATURE,V_TEMP);                // Message for onboard hilink temperature
MyMessage msgCalibration(CHILD_ID_AUTOCALIBRATION,V_STATUS);          // Message for AutoCalibration
MyMessage msgEndStop(CHILD_ID_ENDSTOP,V_STATUS);                      // Simulate endstop
MyMessage msgCurrent(CHILD_ID_CURRENT,V_WATT);                        // Current sensor value debug

MyMessage msgPercent(CHILD_ID_PERCENT,V_DIMMER);                      // should be deprecated in favor of V_PERCENTAGE; COVER type in mysensors already has V_PERCENTAGE. but unfortunately jeedom controller is not up to date yet..

boolean   isMetric                = true;

uint8_t   errorCode               = 0;                                // Node Error code, check table

/* ======================================================================
Function: before()
Purpose : before Mysensors init
Comments: 
====================================================================== */
void before() { 

  analogReference(EXTERNAL);

  digitalWrite(DEBUG_LED, HIGH);
  pinMode(DEBUG_LED, OUTPUT);
  
  // Setup the button
  pinMode(BUTTON_UP,INPUT_PULLUP);
  pinMode(BUTTON_DOWN,INPUT_PULLUP);
  pinMode(BUTTON_STOP,INPUT_PULLUP);

  // After setting up the button, setup debouncer
  debounceUp.attach(BUTTON_UP);
  debounceUp.interval(5);
  debounceDown.attach(BUTTON_DOWN);
  debounceDown.interval(5);
  debounceStop.attach(BUTTON_STOP);
  debounceStop.interval(5);

  // Set relay pins in output mode
  // Make sure relays are off when starting up
  digitalWrite(RELAY_POWER, RELAY_OFF);
  pinMode(RELAY_POWER, OUTPUT);
  digitalWrite(RELAY_UPDOWN, RELAY_OFF);
  pinMode(RELAY_UPDOWN, OUTPUT);
  
}
/* ======================================================================
Function: presentation
Purpose : Send sketch info and present child ids to the GW&Controller 
Comments: 
====================================================================== */
void presentation()  {
  
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(NODE_NAME, RELEASE);

  // Register all id (they will be created as child devices)
  present(CHILD_ID_ROLLERSHUTTER, S_COVER);     // V_UP, V_DOWN, V_STOP, V_PERCENTAGE
  present(CHILD_ID_AUTOCALIBRATION, S_BINARY);  // V_STATUS
  present(CHILD_ID_TEMPERATURE, S_TEMP);        // V_TEMP
  present(CHILD_ID_ENDSTOP, S_BINARY);          // V_STATUS
  present(CHILD_ID_CURRENT, S_POWER);           // V_WATT
  present(CHILD_ID_PERCENT, S_DIMMER);          // V_DIMMER, deprecated, replace V_PERCENTAGE in jeedom controller for the moment..

  isMetric = getConfig().isMetric;
  #ifdef MY_DEBUG_SKETCH 
  Serial.print(F("isMetric: ")); 
  Serial.println(isMetric);  
  #endif  
  
}
/* ======================================================================
Function: setup
Purpose : Arduino Setup 
Comments: 
====================================================================== */
void setup() { 
 
  initDS18x20();  // init hilink temperature monitoring
    
  initShutter(RELAY_POWER, RELAY_UPDOWN);  // init Roller Shutter State Machine
   
  digitalWrite(DEBUG_LED, LOW); 
      
}
/* ======================================================================
Function: loop
Purpose : Arduino main loop 
Comments: 
====================================================================== */
void loop() {

  #ifdef MY_OTA_FIRMWARE_FEATURE
    
  #endif

  // Read buttons, interface
  uint8_t buttonPressed = 0;
  buttonPressed = processButtons(); 
  switch (buttonPressed) {
    case BT_PRESS_UP:
      setPosition(100);
      send(msgUp.set(1), 1);     
      break;
    case BT_PRESS_DOWN:
      setPosition(0);
      send(msgDown.set(1), 1);      
      break;
    case BT_PRESS_STOP:
      ShutterStop();
      send(msgStop.set(1), 1);
      break;            
  }

  // Read current sensor, endstop
  uint16_t acsread = 0;  
  acsread = readAdcWithFilter(ACS712_SENSOR, PS_64); 
  if (acsread < ACS712_LEVELDETECT) {
      // Endstop detected 
      if (getCalibrationState() > 0) setEndStopState(true); 
      else {};
  }
  // for debug, output current sensor
  static uint16_t oldacsread = 0;
  if (abs(oldacsread-acsread)>20) { 
    Serial.print(F("ACS712: READ: ")); 
    Serial.println(acsread); oldacsread = acsread;
    send(msgCurrent.set(acsread));
  }
  
  // Update the roller shutter state machine, for states see shutterSM files
  shutterUpdateSM();   
  // Led ON when relay is on
  if  (digitalRead(RELAY_POWER)==RELAY_ON) digitalWrite(DEBUG_LED, HIGH); else digitalWrite(DEBUG_LED, LOW);
  // todo send % progression for controller widgets NOT TESTED  
//  static uint8_t oldpos=getPosition();
//  if (abs(oldpos-getPosition())>10 && getCalibrationState()==0) { 
//    send(msgShutterPosition.set(getPosition()));
//    send(msgPercent.set(getPosition()));
//  } 

  // Read temperature sensor
  bool tx = false;
  temperatureProcess();      
  float diffTemp = abs(oldTemperature-hilinkTemperature); 
  if  (diffTemp > TEMP_TRANSMIT_THRESHOLD)    tx=true; 
  if  (hilinkTemperature > TEMPERATURE_ALARM) { 
    tx = true; 
    ShutterStop(); 
  } 
  oldTemperature = hilinkTemperature;
  if (tx) send(msgTemperature.set((uint8_t)hilinkTemperature), 1);  // TODO problem sending float to jeedom???weird..need to check  

}
/* ======================================================================
Function: receive
Purpose : Mysensors incomming message  
Comments: 
====================================================================== */
void receive(const MyMessage &message) {
  
  if (message.isAck()) {}
  else {
    // Message received : Open shutters
    if (message.type == V_UP && message.sensor==CHILD_ID_ROLLERSHUTTER) {
      #ifdef MY_DEBUG_SKETCH 
      Serial.println(F("CMD: Up")); 
      #endif 
      setPosition(100);
    }  
       
    // Message received : Close shutters
    if (message.type == V_DOWN && message.sensor==CHILD_ID_ROLLERSHUTTER) {
      #ifdef MY_DEBUG_SKETCH 
      Serial.println(F("CMD: Down")); 
      #endif 
      setPosition(0);
    }
  
    // Message received : Stop shutters motor
    if (message.type == V_STOP && message.sensor==CHILD_ID_ROLLERSHUTTER) {
       #ifdef MY_DEBUG_SKETCH 
       Serial.println(F("CMD: Stop")); 
       #endif 
       ShutterStop(); 
     }     
    
    // Message received : Set position of Rollershutter 
    if (message.type == V_PERCENTAGE && message.sensor==CHILD_ID_ROLLERSHUTTER) {
       if (message.getByte() > 100) setPosition(100); else setPosition(message.getByte());
  
       #ifdef MY_DEBUG_SKETCH 
          Serial.print(F("CMD: Set position to ")); 
          // Write some debug info               
          Serial.print(message.getByte());Serial.println(F("%")); 
       #endif     
     } 
    // Message received : Set position of Rollershutter 
    if (message.type == V_DIMMER && message.sensor==CHILD_ID_PERCENT) {
       if (message.getByte() > 100) setPosition(100); else setPosition(message.getByte());
  
       #ifdef MY_DEBUG_SKETCH 
          Serial.print(F("CMD: Set position to ")); 
          // Write some debug info               
          Serial.print(message.getByte());Serial.println(F("%"));
       #endif     
     }
     
    // Message received : Calibration requested 
    if (message.type == V_STATUS && message.sensor==CHILD_ID_AUTOCALIBRATION) {
       // Write some debug info
       #ifdef MY_DEBUG_SKETCH 
       Serial.println(F("CMD: Autocalibration")); 
       #endif 
       startCalibration();       
     }  
    // Message received : Endstop command received 
    if (message.type == V_STATUS && message.sensor==CHILD_ID_ENDSTOP) {
       // Write some debug info
       #ifdef MY_DEBUG_SKETCH 
       Serial.println(F("CMD: Endstop")); 
       #endif 
       setEndStopState(true);       
     }       
  }
     
}
/* ======================================================================
Function: processButtons
Purpose : Read buttons and update status
Output  : button pressed
Comments: 
====================================================================== */
uint8_t processButtons() {
 
  uint8_t result = BT_PRESS_NONE;
  
  if (debounceUp.update()) { 
    // Button Up change detected
    if (debounceUp.fell()) result = BT_PRESS_UP;               
    else result = BT_PRESS_STOP; // Button released          
  }

  if (debounceDown.update()) {
    // Button Down change detected
    if (debounceDown.fell()) result = BT_PRESS_DOWN;
    else result = BT_PRESS_STOP;
  }

  if (debounceStop.update()) { 
    // Button Stop change detected
    if (debounceStop.fell()) result = BT_PRESS_STOP;
  }
  
  return result;
  
}
/* ======================================================================
Function: readAdcWithFilter
Purpose : Read adc input with basic digital mode filtering
Input   : - adc pin
          - prescal : adc prescaler / if 0, no optimization done
Output  : 10 bit ADC value
Comments: 
====================================================================== */
#define NUM_READS 20

uint16_t readAdcWithFilter(uint8_t adcpin, uint8_t prescal) {
   
   // read multiple values and sort them to take the mode
   int sortedValues[NUM_READS];
   for(int i=0;i<NUM_READS;i++){
     int value = analogRead(adcpin);
     int j;
     if(value<sortedValues[0] || i==0){
        j=0; //insert at first position
     }
     else{
       for(j=1;j<i;j++){
          if(sortedValues[j-1]<=value && sortedValues[j]>=value){
            // j is insert position
            break;
          }
       }
     }
     for(int k=i;k>j;k--){
       // move all values higher than current reading up one position
       sortedValues[k]=sortedValues[k-1];
     }
     sortedValues[j]=value; //insert current reading
   }
   //return scaled mode of 6 values
   uint16_t returnval = 0;
   for(int i=NUM_READS/2-3;i<(NUM_READS/2+3);i++){
     returnval +=sortedValues[i];
   }
   returnval = returnval/6;
   
   return returnval;
   
}
/* *******************************************************************************************************************
 *                                          TEMPERATURE SENSOR MANAGEMENT
 * ******************************************************************************************************************/
uint8_t addr[8];
void initDS18x20() {
  
  uint8_t i;  
  
  Serial.print(F("STEMP: INIT"));
  if ( !ow_ds1820.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ow_ds1820.reset_search();
    delay(250);
    return;
  }
  
  Serial.print(": ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
  Serial.print(F("STEMP: INIT: "));
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  } 
  
}
void temperatureProcess() {
  
  static uint32_t previousTimeRead = millis();
  static uint32_t ds18readingTime = 0;
  
  static bool readit = false;
  
  if ( (millis()-previousTimeRead) > TEMP_MEASURE_INTERVAL) {    
    ow_ds1820.reset();
    ow_ds1820.select(addr);
    ow_ds1820.write(0x44, 1);        // start conversion, with parasite power on at the end  
    
    readit = true; 
    previousTimeRead = millis();
    ds18readingTime = millis();
  }
  if (readit) { 

    uint8_t i;
    uint8_t type_s;
    uint8_t data[12];
  
    float celsius, fahrenheit;

    // 1000ms to let sensors doing his sample, so we don't block execution
    if ( (millis()-ds18readingTime) > 1000) {
  
        Serial.print(F("STEMP: READ: "));
        
        ow_ds1820.reset();
        ow_ds1820.select(addr);    
        ow_ds1820.write(0xBE);         // Read Scratchpad
      
        for ( i = 0; i < 9; i++) {           // we need 9 bytes
          data[i] = ow_ds1820.read();
        }
      
        // Convert the data to actual temperature
        // because the result is a 16 bit signed integer, it should
        // be stored to an "int16_t" type, which is always 16 bits
        // even when compiled on a 32 bit processor.
        int16_t raw = (data[1] << 8) | data[0];
        if (type_s) {
          raw = raw << 3; // 9 bit resolution default
          if (data[7] == 0x10) {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - data[6];
          }
        } else {
          byte cfg = (data[4] & 0x60);
          // at lower res, the low bits are undefined, so let's zero them
          if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
          else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
          else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
          //// default is 12 bit resolution, 750 ms conversion time
        }
        hilinkTemperature = (float)raw / 16.0;
        hilinkTemperature = (isMetric ? hilinkTemperature : hilinkTemperature* 1.8 + 32.0);
  
        Serial.println(hilinkTemperature);
        
        ds18readingTime = millis();
        readit = false;
    }    
  }
}

