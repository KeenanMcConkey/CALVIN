// Robot_FinalSwitch

// INCLUDES
#include <phys253.h>
#include <LiquidCrystal.h>

//STATES
#define PAUSE_STATE 0
#define GATE_STATE 1
#define STOP_IR_STATE 2
#define POST_GATE_STATE 3
#define CLIMB_STATE 4
#define POST_CLIMB_STATE 5 
#define CIRCLE_STATE 6
#define STOP_LINE_STATE 7
#define ZIPLINE_STATE 8
#define FINISH_STATE 9

// ANALOG PINS
#define L_QRD_PIN_IN 1
#define R_QRD_PIN_IN 0
#define L_QRD_PIN_OUT 3
#define R_QRD_PIN_OUT 2
#define IR_PIN_1 5
#define IR_PIN_10 6

// DIGITAL PINS
#define FRONT_SWITCH_PIN_L 0
#define FRONT_SWITCH_PIN_R 6
#define MID_SWITCH_PIN_L 1
#define MID_SWITCH_PIN_R 7

// MOTOR PINS
#define L_MTR_PIN 1
#define R_MTR_PIN 0

// KNOB PINS
#define KNOB_PIN 7

// THRESHOLDS
#define L_TSH 40
#define R_TSH 40 
#define L_TSH_OUT 150
#define R_TSH_OUT 100
#define L_TSH_CLIMB 40
#define R_TSH_CLIMB 40
#define L_TSH_OUT_CLIMB 200
#define R_TSH_OUT_CLIMB 100

// GAINS_2

#define K_P_GATE 40
#define K_D_GATE 350
#define K_P_CLIMB 45
#define K_D_CLIMB 450
#define K_P_POST_CLIMB 38
#define K_D_POST_CLIMB 400
#define K_P_CIRCLE 30
#define K_D_CIRCLE 300
#define MEMORY_ERROR 3
#define CIRCLE_ERROR 2
#define CIRCLE_BLIND_TURN_L 20
#define CIRCLE_BLIND_TURN_R 15

// SPEEDS
#define BASE_SPEED 130
#define CLIMB_SPEED 160
#define POST_CLIMB_SPEED 110
#define CIRCLE_SPEED 75
#define STOP_SPEED 0
#define BACK_SPEED 75
#define REALIGN_SPEED 75
#define ZIPLINE_SPEED 75

// AGENTS
#define AGENT_MIN 1
#define AGENT_MAX 9
#define JUNCTION_LINE 7
#define ZIPLINE_MAX 11

// DELAYS
#define GATE_DELAY 1400
#define PRE_GATE_DELAY 200
#define CHECK_GATE_DELAY 100
#define POST_GATE_DELAY 50
#define CLIMB_DELAY 300
#define POST_CLIMB_DELAY 500
#define PICKUP_DELAY 400
#define PRE_PICKUP_DELAY 100
#define POST_PICKUP_DELAY 50
#define PICKUP_REALIGN_DELAY 250
#define PASS_JUNCTION_DELAY 30
#define REALIGN_DELAY 300
#define TURN_90_DELAY 150
#define PRE_CIRCLE_DELAY 50
#define PRE_ZIPLINE_DELAY 200
#define TURN_ZIPLINE_DELAY 400
#define LIFT_ZIPLINE_DELAY 10000
#define FIND_ZIPLINE_DELAY 2000
#define DROPOFF_ZIPLINE_DELAY 6000
#define PRINT_F 20

// GENERAL SERVO
#define KNOB_MAX 1023.0
#define ANGLE_MAX 180

// CRANE
#define CRANE_PICKUP_ANGLE_L 6
#define CRANE_PICKUP_ANGLE_R 140
#define CRANE_DROP_ANGLE_L 86
#define CRANE_DROP_ANGLE_R 90
#define CRANE_REST_ANGLE 86
#define CRANE_CALIBRATION_ANGLE 180
#define CRANE_REALIGN_ANGLE 120
#define CRANE_ZIPLINE_ANGLE 76

// ARM
#define ARM_REST_ANGLE 10
#define ARM_DRIVING_ANGLE 28
#define ARM_MAX_ANGLE 162
#define ARM_ROTATION_ANGLE 75 // C
#define ARM_PICKUP_ANGLE 5
#define ARM_DROP_ANGLE 25 // C
#define ARM_CALIBRATION_ANGLE 40

// CLAW
#define CLAW_REST_ANGLE 115
#define CLAW_CLOSE_ANGLE 170
#define CLAW_HARD_CLOSE_ANGLE 170
#define CLAW_OPEN_ANGLE 85
#define CLAW_CALIBRATION_ANGLE 75
#define CLAW_DROP_DELAY 500

//BOX
#define CLAW_BOX_OPEN_ANGLE 90
#define ARM_BOX_ANGLE 18
#define ARM_BOX_LIFT_ANGLE 120
#define ARM_BOX_DROP_ANGLE 100

// GLOBAL VARIABLES
static int state = CIRCLE_STATE;
static int lineCount = 0;

static int lastError = 0;
static int derivCount = 0;
static int p = 0;
static int d = 0;
static int error = 0;
static int printCount = 0;

// BASIC 
void startPause() {
  while( !(startbutton()) );
}

void stopPause() {
  while( !(stopbutton()) );
}

void LCDreset() {
  LCD.clear();
  LCD.home();
  LCD.setCursor(0,0);
}

void LCDprintState() {
  LCDreset();
  LCD.print("State: ");
  LCD.print(state);
}

// SERVOS
void clawRestCommand() {
  RCServo0.write( CRANE_DROP_ANGLE_R );
  RCServo1.write( ARM_DRIVING_ANGLE );
  RCServo2.write( CLAW_REST_ANGLE );
}

void clawPickupRestCommand() {
  RCServo0.write( CRANE_DROP_ANGLE_R );
  RCServo1.write( ARM_DROP_ANGLE );
  RCServo2.write( CLAW_REST_ANGLE );
  
}

void rightPickup() {

  RCServo0.write( CRANE_PICKUP_ANGLE_R ); // C
  RCServo2.write( CLAW_OPEN_ANGLE );
  delay(50);
  
  RCServo1.write(ARM_ROTATION_ANGLE);
  delay(PICKUP_DELAY);
  
  RCServo1.write( ARM_PICKUP_ANGLE );
  delay(PICKUP_DELAY);

  RCServo2.write( CLAW_CLOSE_ANGLE ); // C
  delay(PICKUP_DELAY);

  RCServo1.write( ARM_ROTATION_ANGLE );
  delay(PICKUP_DELAY);

  RCServo0.write(CRANE_DROP_ANGLE_R);
  delay(CLAW_DROP_DELAY);

  RCServo1.write(ARM_DROP_ANGLE);
  delay(CLAW_DROP_DELAY);
  RCServo2.write( CLAW_OPEN_ANGLE + 20 );
  delay(200);
  RCServo2.write( CLAW_REST_ANGLE  );
  delay(100);

}

void boxLift() {

  RCServo0.write( CRANE_PICKUP_ANGLE_R ); // C
  delay(50);
  
  RCServo1.write(ARM_ROTATION_ANGLE);
  delay(PICKUP_DELAY);

  //Initialize crane position
  RCServo2.write( CLAW_OPEN_ANGLE );
  delay(PICKUP_DELAY);
  
  RCServo0.write( CRANE_ZIPLINE_ANGLE );
  delay(PICKUP_DELAY);
  
  RCServo1.write( ARM_BOX_ANGLE );
  delay(PICKUP_DELAY);
  
  RCServo2.write( CLAW_CLOSE_ANGLE - 30 ); // C
  delay(PICKUP_DELAY);

  RCServo1.write( ARM_BOX_LIFT_ANGLE );
  delay(PICKUP_DELAY);
}

void boxLetGo() {
  RCServo1.write( ARM_BOX_DROP_ANGLE);
  delay(PICKUP_DELAY);
  
  RCServo2.write( CLAW_BOX_OPEN_ANGLE );
}

//MOTORS

void motorsStop() {
  motor.speed(R_MTR_PIN, STOP_SPEED);
  motor.speed(L_MTR_PIN, STOP_SPEED);
}

void motorsTurn90(int runSpeed) {
  motor.speed(R_MTR_PIN, runSpeed);
  motor.speed(L_MTR_PIN, -1*runSpeed);
}

void motorsForward(int selectSpeed) {
  motor.speed(R_MTR_PIN, selectSpeed);
  motor.speed(L_MTR_PIN, selectSpeed);
}

void motorsBackward(int selectSpeed) {
  motor.speed(R_MTR_PIN, -1*selectSpeed);
  motor.speed(L_MTR_PIN, -1*selectSpeed);
}

void motorsTurnLeft( int averageSpeed ) {
  motor.speed(R_MTR_PIN, averageSpeed + CIRCLE_BLIND_TURN_L);
  motor.speed(L_MTR_PIN, averageSpeed - CIRCLE_BLIND_TURN_L);
}

void motorsTurnRight( int averageSpeed ) {
  motor.speed(R_MTR_PIN, averageSpeed - CIRCLE_BLIND_TURN_R);
  motor.speed(L_MTR_PIN, averageSpeed + CIRCLE_BLIND_TURN_R);
}

// PID

void runPIDFollow(int runSpeed, int kP, int kD, int lTsh, int rTsh) {
  int lQRD = analogRead(L_QRD_PIN_IN);
  int rQRD = analogRead(R_QRD_PIN_IN);

  int recentError = 0;
  int derivCurr = 0;

  if (lQRD > lTsh && rQRD > rTsh) { error = 0; }
  else if (lQRD > lTsh && rQRD < rTsh) { error = -1; }
  else if (lQRD < lTsh && rQRD > rTsh) { error =  1; }
  else if (lQRD < lTsh && rQRD < rTsh) {
    if (lastError > 0)  { error =  MEMORY_ERROR; }
    if (lastError <= 0) { error = -1 * MEMORY_ERROR; }
  }
  
  if (error != lastError) {
    recentError = lastError;
    derivCurr = derivCount;
    derivCount = 1;
  }

  p = kP * error;
  d = (int)(((float)kD * float(error - recentError)) / ((float)(derivCurr + derivCount)));
  int control = p + d;

  motor.speed(R_MTR_PIN, runSpeed - control);
  motor.speed(L_MTR_PIN, runSpeed + control);

  if (printCount == PRINT_F) {
    LCD.setCursor(0,1);
    LCD.print("p: "); LCD.print(p); 
    LCD.print(" d: "); LCD.print(d);

    printCount = 0;
  }
  
  printCount++;
  derivCount++;
  lastError = error;
}

// STOPS

boolean stopIR() { // 10 is true
  int ir1 = analogRead(IR_PIN_1);
  int ir10 = analogRead(IR_PIN_10);

  if (ir1 <= ir10) { return true; }
  else { return false; }
}

boolean stopLine(int lTshIn, int rTshIn, int lTshOut, int rTshOut) {
  int lQRDin = analogRead(L_QRD_PIN_IN);
  int rQRDin = analogRead(R_QRD_PIN_IN);
  int lQRDout = analogRead(L_QRD_PIN_OUT);
  int rQRDout = analogRead(R_QRD_PIN_OUT);

  if (lQRDin > lTshIn && rQRDin > rTshIn && lQRDout > lTshOut && rQRDout > rTshOut)  { 
    return true;
  } else { return false; }
}

void maximizeLastError(int err) {
  if (lastError >= 0) {
    lastError = err;
  } else {
    lastError = -err;
  }
}


// SWITCHES

boolean climbSwitch() {
  int frontSwitchL = digitalRead(FRONT_SWITCH_PIN_L);
  int frontSwitchR = digitalRead(FRONT_SWITCH_PIN_R);
  if( frontSwitchL == HIGH || frontSwitchR == HIGH) { return true; }
  else { return false; }
}

boolean endSwitch() {
  int midSwitchL = digitalRead(MID_SWITCH_PIN_L);
  int midSwitchR = digitalRead(MID_SWITCH_PIN_R);
  if( midSwitchL == HIGH || midSwitchR == HIGH) { return true; }
  else { return false; }  
}

void setup() {
  #include <phys253setup.txt>
  LCDreset();
  clawRestCommand();
  LCD.print("SwitchFinal V1.0");
  LCD.setCursor(0,1); LCD.print("Press START");
  startPause();
}

void loop() {
  if (state == GATE_STATE) {
      LCDprintState();
      
      for (long t = millis(); millis() - t < GATE_DELAY; ) {
        runPIDFollow(BASE_SPEED, K_P_GATE, K_D_GATE, L_TSH, R_TSH);
      }
      
      state = STOP_IR_STATE;
  } else if (state == STOP_IR_STATE) {
      LCDprintState();
      
      motorsBackward(BASE_SPEED);
      delay(PRE_GATE_DELAY);
      
      motorsStop();
      delay(PRE_GATE_DELAY);
      
      while(!stopIR());
      delay(CHECK_GATE_DELAY);
      
      while(stopIR());
      delay(POST_GATE_DELAY);

      state = POST_GATE_STATE;
  } else if (state == POST_GATE_STATE) {
      LCDprintState();
      
      while(!climbSwitch()) {
        runPIDFollow(BASE_SPEED, K_P_GATE, K_D_GATE, L_TSH, R_TSH);
      }

      state = CLIMB_STATE;
  } else if (state == CLIMB_STATE) {
      LCDprintState();
      
      while(!endSwitch()) {
        runPIDFollow(CLIMB_SPEED, K_P_CLIMB, K_D_CLIMB, L_TSH_CLIMB, R_TSH_CLIMB);
      }

      for (long t = millis(); millis() - t < CLIMB_DELAY; ) {
        runPIDFollow(BASE_SPEED, K_P_GATE, K_D_GATE, L_TSH, R_TSH);
      }
      
      state = POST_CLIMB_STATE;
  } else if (state == POST_CLIMB_STATE) {
      LCDprintState();
      
      for (long t = millis(); millis() - t < POST_CLIMB_DELAY; ) {
        runPIDFollow(POST_CLIMB_SPEED, K_P_POST_CLIMB, K_D_POST_CLIMB, L_TSH, R_TSH);
      }

      maximizeLastError(MEMORY_ERROR);
      
      while(!stopLine(L_TSH, R_TSH, L_TSH_OUT, R_TSH_OUT)) {
        runPIDFollow(BASE_SPEED, K_P_GATE, K_D_GATE, L_TSH, R_TSH);
      }

      motorsBackward(CIRCLE_SPEED);
      delay(REALIGN_DELAY);
      
      motorsTurn90(CIRCLE_SPEED);
      delay(TURN_90_DELAY);

      for (long t = millis(); millis() - t < PRE_CIRCLE_DELAY; ) {
        runPIDFollow(CIRCLE_SPEED, K_P_CIRCLE, K_D_CIRCLE, L_TSH, R_TSH);
      }
      
      state = CIRCLE_STATE;
  } else if (state == CIRCLE_STATE) {
      LCDprintState();

      if (lineCount > 1) {
        clawPickupRestCommand();
      }

      while(! stopLine(L_TSH, R_TSH, L_TSH_OUT, R_TSH_OUT)) {
        runPIDFollow(CIRCLE_SPEED, K_P_CIRCLE, K_D_CIRCLE, L_TSH, R_TSH);
      }

      lineCount++;
      
      state = STOP_LINE_STATE;    
  } else if (state == STOP_LINE_STATE) {
      LCDprintState();
      
      if (lineCount > AGENT_MIN && lineCount < AGENT_MAX && lineCount != JUNCTION_LINE) {
        motorsBackward(BACK_SPEED);
        delay(PRE_PICKUP_DELAY);
        motorsStop();
        delay(PRE_PICKUP_DELAY);
        rightPickup();
        delay(POST_PICKUP_DELAY);
        motorsTurnLeft(REALIGN_SPEED);
        delay(PICKUP_REALIGN_DELAY);
        
        lastError = 3; // Goes outside
      }

      else {
        motorsTurnRight(CIRCLE_SPEED);
        delay(PASS_JUNCTION_DELAY);

        lastError = 3;
      }

      if (lineCount < ZIPLINE_MAX) {
        state = CIRCLE_STATE;  
      } else {
        state = ZIPLINE_STATE;
      }
  } else if (state == ZIPLINE_STATE) {
      LCDprintState();
  
      motorsStop();
      delay(PRE_ZIPLINE_DELAY);
      motorsTurn90(CIRCLE_SPEED);
      delay(TURN_ZIPLINE_DELAY);

      motorsBackward(BACK_SPEED);
      delay(PRE_ZIPLINE_DELAY);
      motorsStop();
      delay(PRE_ZIPLINE_DELAY);
      
      boxLift();
      delay(LIFT_ZIPLINE_DELAY);
      motorsForward(ZIPLINE_SPEED);
      
      while(!stopLine(L_TSH, R_TSH, L_TSH_OUT, R_TSH_OUT)); // Do nothing
    
      motorsStop();
      boxLetGo();

      delay(PICKUP_DELAY);
      motorsBackward(ZIPLINE_SPEED);
      delay(FIND_ZIPLINE_DELAY);
      motorsStop();
      
      state = FINISH_STATE;
  } else if (state == FINISH_STATE) {
      LCDprintState();

      while(!stopbutton());
  } else {
      LCDreset();
      LCD.print("ERROR: State");
  }
}
