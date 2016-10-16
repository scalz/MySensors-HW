 /* *****************************************************************************************************************************
 * Rollershutter SM
 * 
 * Created by (2016) Pascal Moreau / Scalz
 *
 * ***************************************************************************************************************************** */
#ifndef shutterSM_h
#define shutterSM_h

// ********************* PIN/RELAYS DEFINES *******************************************
#define SHUTTER_UPDOWN_PIN        19                    // Default pin for relay SPDT, you can change it with : initShutter(SHUTTER_POWER_PIN, SHUTTER_UPDOWN_PIN)
#define SHUTTER_POWER_PIN         18                    // Default pin for relay SPST : Normally open, power off rollershutter motor

#define RELAY_ON                  1                     // ON state for power relay
#define RELAY_OFF                 0
#define RELAY_UP                  0                     // UP state for UPDOWN relay
#define RELAY_DOWN                1                     // UP state for UPDOWN relay

// ********************* EEPROM DEFINES **********************************************
#define EEPROM_OFFSET_SKETCH      512                   //  Address start where we store our data (before is mysensors stuff)
#define EEPROM_POSITION           EEPROM_OFFSET_SKETCH  //  Address of shutter last position
#define EEPROM_TRAVELUP           (EEPROM_POSITION+1)   //  Address of travel time for Up. 16bit value
#define EEPROM_TRAVELDOWN         (EEPROM_TRAVELUP+2)   //  Address of travel time for Down. 16bit value

// ********************* CALIBRATION DEFINES *****************************************
#define START_POSITION            0                     // Shutter start position in % if no parameter in memory

#define TRAVELTIME_UP             23000                 // in ms, time measured between 0-100% for Up. it's initialization value. if autocalibration used, these will be overriden
#define TRAVELTIME_DOWN           23000                 // in ms, time measured between 0-100% for Down
#define TRAVELTIME_ERRORMS        1000                  // in ms,  max ms of error during travel, before reporting it as an error

#define CALIBRATION_SAMPLES       1                     // nb of round during calibration, then average
#define CALIBRATION_TIMEOUT       30000                 // in ms, timeout between 0 to 100% or vice versa, during calibration

// ********************* STATE MACHINE ***********************************************
// defintion of a state structure : transition called first and once, then update is always called
struct shutterState {
  void(*Transition)();         
  void(*Update)();           
};

// definition of the shutter state machine : state & properties
typedef struct {
  shutterState* currentState;   
  uint32_t stateEnter;        
  
  uint32_t  upTimeRef               = TRAVELTIME_UP;
  uint32_t  downTimeRef             = TRAVELTIME_DOWN;
  uint32_t  travelTime              = 0;
  uint8_t   currentPosition         = 0;
  uint8_t   oldPosition             = 0;
  uint8_t   positionRequested       = 0;
  uint8_t   endStop                 = 0;
  uint8_t   calibrationState        = 0;
} shutterSM;

void shutterSwitchSM(shutterState& newState);   // Change the state in the machine
void shutterUpdateSM();                         // Update the state machine (transition once, then update) etc.
uint32_t shutterTimeInState();                  // Time elapsed in state

// ********************* STATES *******************************************************
void ShutterInitializing();             // State 1  : At startup, Initialisation of shutter state machine (load params etc...)
void ShutterProcessingTransition();     // State 2a : Transition before shutter waiting/processing any commands
void ShutterProcessing();               // State 2b : Waiting/processing commands

void ShutterUpTransition();             // Transition called before shutter Up
void ShutterUp();                       // State : Processing Up command
void ShutterDownTransition();           // Transition called before shutter Down
void ShutterDown();                     // State : Processing Down command
void ShutterStop();                     // State : Processing Stop command
void ShutterAutocalibrationInit();      // State : Initialisation of autocalibration + reset of position (position to max) TODO: separate it in two sub states..
void ShutterAutoStepDown();             // State : First pass for time measurement, travel down time 
void ShutterAutoStepUp();               // State : Second pass for travel up time

// ********************* OTHERS ******************************************************* 
void    initShutter(uint8_t powerPin, uint8_t directionPin);  // init Shutter Machine
uint8_t getPosition();                                        // Get current shutter position  
void    setPosition(uint8_t pos);                             // Set shutter position       
void    setEndStopState(bool endstop);                        // Set endstop in machine
void    startCalibration();                                   // Start the calibration
uint8_t getCalibrationState();                                // Get calibration state


void     writeEeprom(uint16_t pos, uint16_t value);           // Write a 16bits max value in eeprom
uint16_t readEeprom(uint16_t pos);                            // Read  a 16bits max value in eeprom



#endif
