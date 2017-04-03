
#include "settings.h"
#include "idle.h"

#define WHEELSPEED_ENABLE

//Pin Assignments
#define SPEAKER           3
const int throttlePositionPin = A0; //Read PWM from this pin
const int engineSpeedPin = 9; //Output RPM to this pin
int engineSpeedval = 0;
float engineSpeedvolt = 0;
#ifdef WHEELSPEED_ENABLE
  const int wheelSpeedLeftPin = 10; 
  const int wheelSpeedRightPin = 11; 
  const int currGearBit0 = 12; 
  const int currGearBit1 = 13; 
#endif

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
#define DECEL_BASE_RATE    5000 //Base engine deceleration rate in RPM/s
#define ACCEL_BASE_RATE    500 //Base engine acceleration rate in RPM/s

#ifdef WHEELSPEED_ENABLE
  const float GEAR_RATIOS[4] = { 26.0/15.0, 21.0/16.0, 21.0/20.0, 21.0/25.0 };
  const float FINAL_DRIVE = 11.0/43.0;
  const float WHEEL_CIRCUMFERENCE = 1.43633616; //Circumference in m
#endif

//Time variables
unsigned long lastUpdate = 0; //The last time RPM was calculated in microseconds (10^-6)
unsigned int engineSpeed = IDLE_RPM;

#ifdef WHEELSPEED_ENABLE
  unsigned int currentGear = 1;
#endif

//Engine sound values
volatile uint16_t currentSmpleRate = BASE_RATE; // Current playback rate, this is adjusted depending on engine RPM
boolean audioRunning = false;                   // Audio state, used so we can toggle the sound system
uint16_t curVolume = 0;                         // Current digi pot volume, used for fade in/out
volatile uint16_t curEngineSample;              // Index of current loaded sample
uint8_t  lastSample;                            // Last loaded sample
volatile int16_t pulseWidth = 0;                // Current pulse width when in PWM mode
boolean managedThrottle = true;                 // Managed mode looks after the digipot if fitted for volume, and adds some mass to the engine
int16_t  currentThrottle = 0;                   // 0 - 1000, a top value of 1023 is acceptable

void setup() {
  Serial.begin(9600);
  pinMode(throttlePositionPin, INPUT);
  
  pinMode(engineSpeedPin, OUTPUT);
  //analogWrite(engineSpeedPin, 0);

  #ifdef WHEELSPEED_ENABLE
    pinMode(currGearBit0, INPUT);
    pinMode(currGearBit1, INPUT);
    pinMode(wheelSpeedLeftPin, OUTPUT);
    pinMode(wheelSpeedRightPin, OUTPUT);
    //analogWrite(wheelSpeedLeftPin, 0);
    //analogWrite(wheelSpeedRightPin, 0);
  #endif
  // engine sound pwm in setup, for a standard servo pulse
  /*pinMode(2, INPUT); // We don't want INPUT_PULLUP as the 5v may damage some receivers!
  attachInterrupt(0, getPulsewidth, CHANGE);
  // setup complete, so start making sounds
  startPlayback();*/
}

void loop() {
  engineSpeedval = analogRead(throttlePositionPin); // read the input pin
  engineSpeedvolt =(5.0 * engineSpeedval) / 1023;
  engineSpeedval = 255 * (engineSpeedvolt / 5);
  analogWrite(engineSpeedPin, engineSpeedval);
  
  //Engine Speed Calculations
  unsigned long delta = micros() - lastUpdate;
  lastUpdate = micros();
  engineSpeed = calculate_RPM(engineSpeed, get_throttle_position(), delta);
//  Serial.print("R"); Serial.println(engineSpeed);
  Serial.print("Engine Speed: ");
  Serial.print(engineSpeed);
  Serial.print("      ");

  #ifdef WHEELSPEED_ENABLE
    //Wheel Speed Calculations
    float wheelSpeed;
    //Assume the Rekluse clutch works properly
    set_curr_gear();
    float ws = 0;
    if(engineSpeed >= 4000) {
      wheelSpeed = engineSpeed * GEAR_RATIOS[currentGear - 1] * FINAL_DRIVE * WHEEL_CIRCUMFERENCE * 3.6; //Km/h
      ws =  (engineSpeed * 0.00595 * WHEEL_CIRCUMFERENCE) / GEAR_RATIOS[currentGear - 1]; //Km/h
    } else {
      wheelSpeed = 0;
    }
    float wheelOutputValue = (float)wheelSpeed * 255.0 / 50000.0; //Scaled between 0 and 140 km/h
//    Serial.print("S"); Serial.println(ws);
    analogWrite(wheelSpeedLeftPin, wheelOutputValue);
    analogWrite(wheelSpeedRightPin, wheelOutputValue);
  Serial.println();
  #endif
  //doPwmThrottle();
  delayMicroseconds(500);
}

void set_curr_gear(){
  unsigned int gear = 1;
  if(digitalRead(currGearBit0) == HIGH){
    gear+=1;
  }
  if(digitalRead(currGearBit1) == HIGH){
    gear+=2;
  }
  currentGear = gear;
  Serial.print("current gear:");
  Serial.print(currentGear);
  Serial.print("    "); 
//  Serial.print("G");
//  Serial.println(currentGear);  
}

float get_throttle_position() {
  int analogReading = analogRead(throttlePositionPin); //Pin reads up to 5V, but max we supply is 3.3V
//  Serial.print("throttle position:");
//  Serial.print(analogReading);
//  Serial.print("    ");
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

//    Serial.print("Target: "); Serial.print(targetRPM); Serial.print("    ");

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

//    Serial.print("Delta: "); Serial.print(delta); Serial.print("    ");

  return currentRPM + delta;
}

void doPwmThrottle(){

  if(managedThrottle){
    if(pulseWidth > 800 && pulseWidth < 2200){ // check if the pulsewidth looks like a servo pulse
      if(pulseWidth < 1000) pulseWidth = 1000; // Constrain the value
      if(pulseWidth > 2000) pulseWidth = 2000;

      if(pulseWidth > 1520) currentThrottle = (pulseWidth - 1500) *2;  // make a throttle value from the pulsewidth 0 - 1000
      else if(pulseWidth < 1470) currentThrottle = abs( (pulseWidth - 1500) *2);
      else currentThrottle = 0;
    }
  }
  else {
    if(pulseWidth > 800 && pulseWidth < 2200){ // check if the pulsewidth looks like a servo pulse
      if(pulseWidth < 1000) pulseWidth = 1000; // Constrain the value
      if(pulseWidth > 2000) pulseWidth = 2000;

      if(pulseWidth > 1520) currentThrottle = (pulseWidth - 1500) *2;  // make a throttle value from the pulsewidth 0 - 1000
      else if(pulseWidth < 1470) currentThrottle = abs( (pulseWidth - 1500) *2);
      else currentThrottle = 0;
      currentSmpleRate = F_CPU / (BASE_RATE + long(currentThrottle * TOP_SPEED_MULTIPLIER));
    }
  }

}

void startPlayback()
{
  pinMode(SPEAKER, OUTPUT);
  audioRunning = true;

  // Set up Timer 2 to do pulse width modulation on the speaker pin.
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));                         // Use internal clock (datasheet p.160)

  TCCR2A |= _BV(WGM21) | _BV(WGM20);                        // Set fast PWM mode  (p.157)
  TCCR2B &= ~_BV(WGM22);

  TCCR2A = (TCCR2A | _BV(COM2B1)) & ~_BV(COM2B0);           // Do non-inverting PWM on pin OC2B (p.155)
  TCCR2A &= ~(_BV(COM2A1) | _BV(COM2A0));                   // On the Arduino this is pin 3.
  TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10); // No prescaler (p.158)

  OCR2B = pgm_read_byte(&idle_data[0]);                     // Set initial pulse width to the first sample.

  // Set up Timer 1 to send a sample every interrupt.
  cli();

  TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);             // Set CTC mode (Clear Timer on Compare Match) (p.133)
  TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));             // Have to set OCR1A *after*, otherwise it gets reset to 0!

  TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10); // No prescaler (p.134)

  OCR1A = F_CPU / BASE_RATE;                                // Set the compare register (OCR1A).
                                                            // OCR1A is a 16-bit register, so we have to do this with
                                                            // interrupts disabled to be safe.

  TIMSK1 |= _BV(OCIE1A);                                   // Enable interrupt when TCNT1 == OCR1A (p.136)

  lastSample = pgm_read_byte(&idle_data[idle_len-1]);
  curEngineSample = 0;
  sei();


  uint8_t target = map(currentThrottle, 0, 1023, VOL_MIN, VOL_MAX); // Fadein the volume pot
  for(uint8_t i = 0; i < target; i ++){
    curVolume = i;
    writePot(curVolume);
    delay(1);
  }
}


void stopPlayback()
{
  // Fadeout the volume pot
  for(uint8_t i = curVolume; i > 0; i--){
    curVolume = i;
    writePot(i);
    delay(1);
  }

  audioRunning = false;

  TIMSK1 &= ~_BV(OCIE1A); // Disable playback per-sample interrupt.
  TCCR1B &= ~_BV(CS10);   // Disable the per-sample timer completely.
  TCCR2B &= ~_BV(CS10);   // Disable the PWM timer.

  digitalWrite(SPEAKER, LOW);
}

// Uses a pin change interrupt and micros() to get the pulsewidth at pin 2
void getPulsewidth(){
  unsigned long currentMicros = micros();
  boolean currentState = digitalRead(2);

  static unsigned long prevMicros = 0;
  static boolean lastState = LOW;

  if(lastState == LOW && currentState == HIGH){      // Rising edge
    prevMicros = currentMicros;
    lastState = currentState;
  }
  else if(lastState == HIGH && currentState == LOW){ // Falling edge
    pulseWidth = currentMicros - prevMicros;
    lastState = currentState;
  }

}


// This is the main playback interrupt, keep this nice and tight!!
ISR(TIMER1_COMPA_vect) {
  OCR1A = currentSmpleRate;

  if (curEngineSample >= idle_len) { // Loop the sample
    curEngineSample = 0;
  }

  OCR2B = pgm_read_byte(&idle_data[curEngineSample]);

  ++curEngineSample;

}

void writePot(uint8_t data){
  // This function should get a value from 0 - 127
  // It would be trivial to convert this to work with
  // an I2C device.

  if(data > VOL_MAX) data = VOL_MAX; // cap it just in case

  digitalWrite(POT_CS, LOW);
  shiftOut(POT_SDO, POT_SCK, MSBFIRST, 0x00);
  shiftOut(POT_SDO, POT_SCK, MSBFIRST, data);
  digitalWrite(POT_CS, HIGH);

}
