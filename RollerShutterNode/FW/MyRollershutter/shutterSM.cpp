 /* *****************************************************************************************************************************
 * Rollershutter SM
 * 
 * Created by (2016) Pascal Moreau / Scalz
 * 
 * ***************************************************************************************************************************** */
#include <Arduino.h>
#include <EEPROM.h>

#include "shutterSM.h"

/* *******************************************************************************************************************
 *                                          SM  
 * ******************************************************************************************************************/
static shutterSM _shutterSM;

static uint8_t _powerPin = SHUTTER_POWER_PIN;
static uint8_t _directionPin = SHUTTER_UPDOWN_PIN;

void shutterSwitchSM(shutterState& newState) {
  // Change state if needed
  if (_shutterSM.currentState != &newState) _shutterSM.currentState = &newState;
  // Transition event
  if (_shutterSM.currentState->Transition) _shutterSM.currentState->Transition();
  // save time
  _shutterSM.stateEnter = millis();
}

uint32_t shutterTimeInState() {
  return millis() - _shutterSM.stateEnter;
}

void shutterUpdateSM(){  
  if (_shutterSM.currentState->Update) _shutterSM.currentState->Update();
}

/* *******************************************************************************************************************
 *                                          ROLLER SHUTTER STATES MACHINE  
 * ******************************************************************************************************************/
static shutterState shInit        = { ShutterInitializing, NULL };
static shutterState shProcess     = { ShutterProcessingTransition, ShutterProcessing };
static shutterState shUp          = { ShutterUpTransition, ShutterUp };
static shutterState shDown        = { ShutterDownTransition, ShutterDown };
static shutterState shStop        = { NULL, ShutterStop };
static shutterState shCalibration = { NULL, ShutterAutocalibrationInit };
static shutterState shCalDown     = { NULL, ShutterAutoStepDown };
static shutterState shCalUp       = { NULL, ShutterAutoStepUp };

/* ======================================================================
Function: ShutterInitializing
Purpose : State 1  : At startup, Initialisation of shutter state machine (load params etc...)
Comments: 
====================================================================== */
void ShutterInitializing() {
        
  #ifdef MY_DEBUG_SKETCH
  Serial.println(F("SHUTTER: INIT")); 
  #endif 
    
  // Shutters initialisation
  _shutterSM.positionRequested = START_POSITION; 
  
  // Read Eeprom for config : byte1 = autocalib(if 0xff it has never done yet), byte2= previous position stored, byte3-4= traveluptime, byte 5-6= traveldowntime
  _shutterSM.currentPosition = EEPROM.read(EEPROM_POSITION);
    
  if (_shutterSM.currentPosition>100) _shutterSM.currentPosition = 0; else _shutterSM.positionRequested = _shutterSM.currentPosition; // no previous position stored
  _shutterSM.oldPosition = _shutterSM.currentPosition;
    
  _shutterSM.upTimeRef   = readEeprom(EEPROM_TRAVELUP);
  _shutterSM.downTimeRef = readEeprom(EEPROM_TRAVELDOWN); 
   
  if (_shutterSM.upTimeRef   == 0xffff) _shutterSM.upTimeRef = TRAVELTIME_UP;     // no previous position stored
  if (_shutterSM.downTimeRef == 0xffff) _shutterSM.downTimeRef = TRAVELTIME_DOWN; // no previous position stored  
    
  #ifdef MY_DEBUG_SKETCH 
  Serial.println(F( "========= Node EEprom loading =========="));
  Serial.print(     "Current Position    : "); Serial.println(_shutterSM.currentPosition); 
  Serial.print(     "Up Traveltime Ref   : "); Serial.println(_shutterSM.upTimeRef);
  Serial.print(     "Down Traveltime Ref : "); Serial.println(_shutterSM.downTimeRef);
  Serial.print(     "Requested position  : "); Serial.println(_shutterSM.positionRequested);
  Serial.println(F( "========================================"));  
  #endif 
    
  shutterSwitchSM(shProcess); 
  
}
/* ======================================================================
Function: ShutterProcessingTransition
Purpose : State 2a : Transition before shutter waiting/processing any commands
Comments: 
====================================================================== */
void ShutterProcessingTransition() {

  #ifdef MY_DEBUG_SKETCH
  Serial.println(F("SHUTTER: PROCESS")); 
  #endif   
   
}
/* ======================================================================
Function: ShutterProcessing
Purpose : State 2b : Waiting/processing commands
Comments: 
====================================================================== */
void ShutterProcessing() {  
  
  if (_shutterSM.oldPosition != _shutterSM.currentPosition) _shutterSM.oldPosition = _shutterSM.currentPosition;
  
  if (_shutterSM.currentPosition < _shutterSM.positionRequested) shutterSwitchSM(shUp);
  else if (_shutterSM.currentPosition>_shutterSM.positionRequested) shutterSwitchSM(shDown);

}
/* ======================================================================
Function: ShutterUpTransition
Purpose : Transition called before shutter Up
Comments: 
====================================================================== */
void ShutterUpTransition() {

  _shutterSM.travelTime = (_shutterSM.upTimeRef / 100) * ( abs(_shutterSM.currentPosition - _shutterSM.positionRequested) );    
  
  #ifdef MY_DEBUG_SKETCH      
  Serial.print(F("SHUTTER: UP:"));
  Serial.print(F(" Position = "));Serial.print(_shutterSM.currentPosition); 
  Serial.print(F(" Requested = "));Serial.print(_shutterSM.positionRequested);
  Serial.print(F(" Traveltime = "));Serial.println(_shutterSM.travelTime);
  #endif      
  
}
/* ======================================================================
Function: ShutterUp
Purpose : State : Processing Up command
Comments: 
====================================================================== */
void ShutterUp() { 
  
  // 0=RELAY_UP, 1= RELAY_DOWN
  digitalWrite(_directionPin, RELAY_UP);
  // Power ON motor 
  digitalWrite(_powerPin, RELAY_ON);      

  _shutterSM.currentPosition=_shutterSM.oldPosition+(shutterTimeInState()*100/_shutterSM.upTimeRef);
  
  // targetting % Up
  if (shutterTimeInState() > _shutterSM.travelTime) {
    // Target position achieved
    #ifdef MY_DEBUG_SKETCH
    Serial.print(F("SHUTTER: UP: Elapsed time = "));Serial.print(shutterTimeInState());
    Serial.print(F(" Position = "));Serial.println(_shutterSM.currentPosition); 
    #endif   
    _shutterSM.currentPosition=_shutterSM.positionRequested;
    shutterSwitchSM(shStop);   
  }   

  if ( shutterTimeInState() > (_shutterSM.upTimeRef + TRAVELTIME_ERRORMS) && _shutterSM.positionRequested<100 ) {
    // error++ // we have a timeout : position requested was <100% but motor current shutdown has not been detected!
    // we send error info, and ask for a calibration as we don't know where we are !!!
    _shutterSM.currentPosition=100; // we suppose we are at 100%
    shutterSwitchSM(shStop);   
  }
  
}
/* ======================================================================
Function: ShutterDownTransition
Purpose : Transition called before shutter Down
Comments: 
====================================================================== */
void ShutterDownTransition() {

  _shutterSM.travelTime = (_shutterSM.downTimeRef/100)*(_shutterSM.currentPosition-_shutterSM.positionRequested);
      
  #ifdef MY_DEBUG_SKETCH
  Serial.print(F("SHUTTER: DOWN:"));
  Serial.print(F(" Position = "));Serial.print(_shutterSM.currentPosition);
  Serial.print(F(" Requested = "));Serial.print(_shutterSM.positionRequested);
  Serial.print(F(" Traveltime = "));Serial.println(_shutterSM.travelTime);
  #endif  
    
}
/* ======================================================================
Function: ShutterDown
Purpose : State : Processing Down command
Comments: 
====================================================================== */
void ShutterDown() {  
   
  // 0=RELAY_UP, 1= RELAY_DOWN
  digitalWrite(_directionPin, RELAY_DOWN);
  // Power ON motor 
  digitalWrite(_powerPin, RELAY_ON);  

  _shutterSM.currentPosition = _shutterSM.oldPosition-(shutterTimeInState()*100/_shutterSM.downTimeRef);

  // targetting % Down
  if (shutterTimeInState() > _shutterSM.travelTime) {
    // Target position achieved
    #ifdef MY_DEBUG_SKETCH 
    Serial.print(F("SHUTTER: DOWN: Elapsed time = "));Serial.print(shutterTimeInState());
    Serial.print(F(" Position = "));Serial.println(_shutterSM.currentPosition);
    #endif  
    _shutterSM.currentPosition = _shutterSM.positionRequested;   
    shutterSwitchSM(shStop);   
  }   

  if ( shutterTimeInState() > (_shutterSM.downTimeRef + TRAVELTIME_ERRORMS) && _shutterSM.positionRequested<100 ) {
    // error++ // we have a timeout : position requested was <100% but motor current shutdown has not been detected!
    // todo : send error info, and ask for a calibration as we don't know where we are !!!
    _shutterSM.currentPosition=0; // we suppose we are at 0% 
    shutterSwitchSM(shStop);   
  }
  
}
/* ======================================================================
Function: ShutterStop
Purpose : State : Processing Stop command
Comments: 
====================================================================== */
void ShutterStop() {
  
  #ifdef MY_DEBUG_SKETCH 
  Serial.print(F("SHUTTER: STOP: Position = "));Serial.println(_shutterSM.currentPosition);
  #endif 
  
  // Power OFF motor 
  digitalWrite(_powerPin, RELAY_OFF);  
  digitalWrite(_directionPin, RELAY_OFF);

  _shutterSM.positionRequested = _shutterSM.currentPosition;
  _shutterSM.oldPosition = _shutterSM.currentPosition;
  
  EEPROM.write(EEPROM_POSITION, _shutterSM.currentPosition); // save new position in eeprom

  if (_shutterSM.calibrationState==1) _shutterSM.calibrationState = 0;
  shutterSwitchSM(shProcess);
}

/* ***********************************************************************
 * Autocalibration procedure
 * 1) Init
 * 2) We completely open the shutters
 * 3) 100% closing with measuring the travel time, store value
 * 4) 100% opening with measuring the travel time, store value
 * 5) Do it for CALIBRATION_SAMPLES times and do an average
 * 6) go to main shutter process thread
 * ***********************************************************************/
 
 /* ======================================================================
Function: ShutterAutocalibrationInit
Purpose : State : Initialisation of autocalibration + reset of position (position to max) TODO: separate it in two sub states..
Comments: 
====================================================================== */
void ShutterAutocalibrationInit() { 
  
  _shutterSM.calibrationState   = 1;
  _shutterSM.positionRequested  = 0;  
  // Calibration init : we completely open the shutter before calibrating  
  // 0=RELAY_UP, 1= RELAY_DOWN
  digitalWrite(_directionPin, RELAY_UP);
  // Power ON motor 
  digitalWrite(_powerPin, RELAY_ON);  

  if (_shutterSM.endStop==1) {
    // Endstop detected 
    Serial.println("1st"); 
    _shutterSM.endStop = 0;
    shutterSwitchSM(shCalDown);
  }
  if (shutterTimeInState() > CALIBRATION_TIMEOUT && (!_shutterSM.endStop) ) {
    // error++ // we have a timeout and motor current shutdown/endstop has not been detected!
    // we report error info, calibration not complete. Multiple reason like TRAVELTIME_DOWN not well defined etc..

    _shutterSM.upTimeRef          = TRAVELTIME_UP;
    _shutterSM.downTimeRef        = TRAVELTIME_DOWN;
    _shutterSM.currentPosition    = 100; // we suppose we are at 100% 
    _shutterSM.positionRequested  = _shutterSM.currentPosition;
    shutterSwitchSM(shStop);  
  }   
    
}
 /* ======================================================================
Function: ShutterAutoStepDown
Purpose : State : First pass for time measurement, travel down time 
Comments: 
====================================================================== */
void ShutterAutoStepDown() { 

  // Calibration 1st step : we measure travel time for Down

  // 0=RELAY_UP, 1= RELAY_DOWN
  digitalWrite(_directionPin, RELAY_DOWN);
  // Power ON motor 
  digitalWrite(_powerPin, RELAY_ON); 
    
  if (_shutterSM.endStop==1) {
    // Endstop detected 
    _shutterSM.downTimeRef = shutterTimeInState();
    Serial.print(F("SHUTTER: Time elapsed 1st travel = "));Serial.println(_shutterSM.downTimeRef);     
    _shutterSM.endStop = 0;
    shutterSwitchSM(shCalUp);
  }
  if (shutterTimeInState() > CALIBRATION_TIMEOUT && (!_shutterSM.endStop) ) {
    // error++ // we have a timeout and motor current shutdown/endstop has not been detected!
    // we report error info, calibration not complete. Multiple reason like TRAVELTIME_DOWN not well defined etc..
    
    _shutterSM.currentPosition    = 0;                          
    _shutterSM.positionRequested  = _shutterSM.currentPosition;
    shutterSwitchSM(shStop);    
  }  
      
}
/* ======================================================================
Function: ShutterAutoStepUp
Purpose : State : Second pass for travel up time
Comments: 
====================================================================== */
void ShutterAutoStepUp() { 

  // Calibration 2nd step : we measure travel time for Up

  // 0=RELAY_UP, 1= RELAY_DOWN
  digitalWrite(_directionPin, RELAY_UP);
  // Power ON motor 
  digitalWrite(_powerPin, RELAY_ON); 

  if (_shutterSM.endStop==1) {
    // Endstop detected 
    _shutterSM.upTimeRef = shutterTimeInState();
    
    writeEeprom(EEPROM_TRAVELUP, _shutterSM.upTimeRef);
    writeEeprom(EEPROM_TRAVELDOWN, _shutterSM.downTimeRef);    
    
    Serial.print(F("SHUTTER: Time elapsed 2nd travel = "));Serial.println(_shutterSM.upTimeRef);
    _shutterSM.currentPosition    = 100;
    _shutterSM.positionRequested  = _shutterSM.currentPosition;

    _shutterSM.endStop = 0;
    shutterSwitchSM(shStop);
  }  
  if (shutterTimeInState() > CALIBRATION_TIMEOUT && (!_shutterSM.endStop) ) {
    // error++ // we have a timeout and motor current shutdown/endstop has not been detected!
    // todo report error info, calibration not complete. Multiple reason like TRAVELTIME_DOWN not well defined etc..
    
    _shutterSM.currentPosition    = 100; // we suppose we are at 100% 
    _shutterSM.positionRequested  = _shutterSM.currentPosition;
    shutterSwitchSM(shStop);   
  }     
  
}

/* *******************************************************************************************************************
 *                                          ROLLER SHUTTER Get/Set  
 * ******************************************************************************************************************/
 
/* ======================================================================
Function: initShutter
Purpose :  init Shutter Machine
Input   : - powerPin relay SPST
          - directionPin relay SPDT
Comments: 
====================================================================== */
void initShutter(uint8_t powerPin, uint8_t directionPin) {
  _powerPin = powerPin;
  _directionPin = directionPin; 
  shutterSwitchSM(shInit);
}
/* ======================================================================
Function: getPosition
Purpose : Get current shutter position  
Input   : -
Output  : - current shutter position
Comments: 
====================================================================== */
uint8_t getPosition() {
  return _shutterSM.currentPosition;
}
/* ======================================================================
Function: setPosition
Purpose : Set shutter position 
Input   : - pos : position
Output  : 
Comments: 
====================================================================== */
void setPosition(uint8_t pos) {
  _shutterSM.positionRequested = pos;
  shutterSwitchSM(shProcess); 
}
/* ======================================================================
Function: startCalibration
Purpose : Start the calibration  
Comments: 
====================================================================== */
void startCalibration() {  
  shutterSwitchSM(shCalibration);
}
/* ======================================================================
Function: getCalibrationState
Purpose : Get calibration state
Input   : - 
Output  : - calibrationState
Comments: 
====================================================================== */
uint8_t getCalibrationState(){
  return _shutterSM.calibrationState; 
}
/* ======================================================================
Function: setEndStopState
Purpose : Set endstop in machine  
Comments: 
====================================================================== */
void setEndStopState(bool endstop) {
  _shutterSM.endStop = endstop;
}
 
/* ======================================================================
Function: writeEeprom
Purpose : Write uint16_t values in eeprom for (max 65535)
Input   : - pos : start position in eeprom
          - value : 16bits value to write 
Comments: 
====================================================================== */
void writeEeprom(uint16_t pos, uint16_t value) {
  
    // function for saving the values to the internal EEPROM
    // value = the value to be stored (as int)
    // pos = the first byte position to store the value in
    // only two bytes can be stored with this function (max 32.767)
    EEPROM.write(pos, ((uint16_t)value >> 8 ));
    pos++;
    EEPROM.write(pos, (value & 0xff));
    
}
/* ======================================================================
Function: readEeprom
Purpose : read uint16_t values in eeprom for (max 65535)
Input   :  - pos : start position in eeprom
Output  :  - return 16bits value
Comments: 
====================================================================== */
uint16_t readEeprom(uint16_t pos) {
  
    // function for reading the values from the internal EEPROM
    // pos = the first byte position to read the value from 
    uint16_t hiByte;
    uint16_t loByte;

    hiByte = EEPROM.read(pos) << 8;
    pos++;
    loByte = EEPROM.read(pos);
    return (hiByte | loByte);
    
}


