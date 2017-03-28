//#define WHEELSPEED_ENABLE

//Car Parameters
const float MIN_TPS = 12.0; //The minimum TPS to start accelerating
const float WOT_THRESHOLD = 70.0; //The throttle position at which no steady-state RPM will be reached

const float REDLINE_TORQUE = 10.0;
const float IDLE_TORQUE = 17.0;
const float PEAK_TORQUE = 25.0;

#define IDLE_RPM          3500 //Lowest RPM we should hit
#define LO_FLAT_RPM       5000 //Start RPM where torque levels out
#define HI_FLAT_RPM       8000 //End RPM where torque levels out
#define MAX_SS_RPM        9500 //Highest steady-state RPM
#define REDLINE_RPM      10500 //Highest RPM we should hit
#define POWER_CONSTANT   33333 //Time to gain 500 RPM with 11.6lb-ft in microseconds (10^-6)
#define DECEL_BASE_RATE    800 //Base engine deceleration rate in RPM/s
#define ACCEL_BASE_RATE    500 //Base engine acceleration rate in RPM/s

#ifdef WHEELSPEED_ENABLE
  const float GEAR_RATIOS[4] = { 26.0/15.0, 21.0/16.0, 21.0/20.0, 21.0/25.0 };
  const float FINAL_DRIVE = 11.0/43.0;
  const float WHEEL_CIRCUMFERENCE = 1.43633616; //Circumference in m
#endif

//Pin Assignments
const int throttlePositionPin = A0; //Read PWM from this pin
const int engineSpeedPin = 9; //Output RPM to this pin

#ifdef WHEELSPEED_ENABLE
  const int wheelSpeedPin = 10; //Output speed to this pin
  const int upshiftPin = 12; //Upshift button
  const int downshiftPin = 13; //Downshift button
#endif

//Time variables
unsigned long lastUpdate = 0; //The last time RPM was calculated in microseconds (10^-6)
unsigned int engineSpeed = IDLE_RPM;

#ifdef WHEELSPEED_ENABLE
  unsigned int currentGear = 1;
#endif

void setup() {
  Serial.begin(9600);
  pinMode(throttlePositionPin, INPUT);
  
  pinMode(engineSpeedPin, OUTPUT);
  analogWrite(engineSpeedPin, 0);

  #ifdef WHEELSPEED_ENABLE
    pinMode(upshiftPin, INPUT);
    pinMode(downshiftPin, INPUT);

    pinMode(wheelSpeedPin, OUTPUT);
    analogWrite(wheelSpeedPin, 0);
  #endif
}

void loop() {
  //Engine Speed Calculations
  unsigned long delta = micros() - lastUpdate;
  lastUpdate = micros();
  engineSpeed = calculate_RPM(engineSpeed, get_throttle_position(), delta);
  
  //float engineOutputValue = (float)engineSpeed * 255.0 / 11000.0; //Scaled between 0 and 11000 RPM
  //analogWrite(engineSpeedPin, engineOutputValue);
  Serial.print("Engine Speed: ");
  Serial.println(engineSpeed);

  #ifdef WHEELSPEED_ENABLE
    //Wheel Speed Calculations
    float wheelSpeed;
    //Assume the Rekluse clutch works properly
    if(engineSpeed >= 4000) {
      wheelSpeed = engineSpeed * GEAR_RATIOS[currentGear - 1] * FINAL_DRIVE * WHEEL_CIRCUMFERENCE * 3.6; //Km/h
    } else {
      wheelSpeed = 0;
    }
  
    float wheelOutputValue = (float)wheelSpeed * 255.0 / 140.0; //Scaled between 0 and 140 km/h
    analogWrite(wheelSpeedPin, wheelOutputValue);
  #endif

  delayMicroseconds(500);
}

float get_throttle_position() {
  int analogReading = analogRead(throttlePositionPin); //Pin reads up to 5V, but max we supply is 3.3V

  return (float)analogReading * 100.0 / 675.0; //675/1024 is approximately 3.3/5.0
}

unsigned int calculate_RPM(unsigned int currentRPM, float throttlePosition, unsigned long timeDelta) {
  int delta = 0;
  
  if(throttlePosition <= MIN_TPS) {
    //Assume idle or returning to idle
    if(currentRPM <= IDLE_RPM) {
      //Assume we can't stall the engine
      return IDLE_RPM;
    } else {
      //Decelerate engine at 200 RPM/s
      delta = -DECEL_BASE_RATE * ((float)timeDelta / 1000000.0);
    }
  } else {
    unsigned int targetRPM;
    
    if(throttlePosition < WOT_THRESHOLD) {
      targetRPM = IDLE_RPM + (throttlePosition - MIN_TPS) * (MAX_SS_RPM - IDLE_RPM) / (WOT_THRESHOLD - MIN_TPS);
    } else {
      targetRPM = REDLINE_RPM;
    }

    Serial.print("Target: "); Serial.print(targetRPM); Serial.print("    ");

    if(currentRPM < targetRPM) {
      //Accelerate at scaled amount of WOT torque
      float torque;
      
      //Really crappy approximation of WR450 torque curve
      if(currentRPM < 5000) {
        torque = IDLE_TORQUE + ((float)currentRPM - IDLE_RPM) * (PEAK_TORQUE - IDLE_TORQUE) / (LO_FLAT_RPM - IDLE_RPM);
      } else if(currentRPM > 8000) {
        torque = PEAK_TORQUE + ((float)currentRPM - REDLINE_RPM) * (PEAK_TORQUE - REDLINE_TORQUE) / (LO_FLAT_RPM - REDLINE_RPM);
      } else {
        torque = PEAK_TORQUE;
      }
  
      delta = ACCEL_BASE_RATE * ((float)timeDelta / (float)POWER_CONSTANT) * (torque / 11.6) * (throttlePosition / 100.0);
  
      if((currentRPM + delta) >= targetRPM) {
        delta = targetRPM - currentRPM;
      }
    } else if(currentRPM > targetRPM) {
      //Decelerate at scaled amount based on distance from target and amount of throttle applied
      delta = -DECEL_BASE_RATE * (float)(currentRPM - targetRPM > 1000 ? 1.0 : (1.0 - (float)(1000.0 - (currentRPM - targetRPM)) / 1500.0))
                               * (float)(1.0 - (throttlePosition - MIN_TPS) / (100.0 - MIN_TPS))
                               * ((float)timeDelta / 1000000.0);
    } else {
      delta = 0;
    }
  }

    Serial.print("Delta: "); Serial.print(delta); Serial.print("    ");

  return currentRPM + delta;
}










