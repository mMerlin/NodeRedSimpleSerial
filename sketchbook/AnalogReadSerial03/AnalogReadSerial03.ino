/*
  AnalogReadSerial HPD 2020-06-11

  Reads analog input on pin A0 and A1, prints the result to the Serial Monitor.
  Watch serial input and change LED state for specific 'command' characters

  Restructure to make methods responsible for their own timing, and minimize
  global variables
*/

const PROGMEM unsigned int LED_PIN = 3; // pin digital pin controlling LED state
const PROGMEM unsigned int ERROR_PIN = LED_BUILTIN; // show that an unrecognized command was received
const PROGMEM unsigned long BAUD_RATE = 9600; // serial communication speed
const PROGMEM float MAX_VOLTAGE = 5.0; // voltage the board is running at
const PROGMEM unsigned int MAX_MEASURE = 1024; // maximum analog reading
const PROGMEM unsigned long MEASURE_INTERVAL = 5000; // 5 seconds

void setup() {
  // initialize digital output pins
  pinMode(ERROR_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(BAUD_RATE);     // initialize serial communication
} // end of setup()

void sendAnalog(int sourcePin) {
  // get the current value for the specified source pin
  int sensorValue = analogRead(sourcePin);
  // print (to serial) the measured value
  Serial.print(sensorValue);
  Serial.print(F(","));
  // convert the measurement to voltage
  float voltage = sensorValue * MAX_VOLTAGE / MAX_MEASURE;
  Serial.print(voltage);
} // end of sendAnalog()

void doMeasurement(const unsigned long timeRef) {
  static unsigned long lastMsg = 0;
  if (timeRef - lastMsg < MEASURE_INTERVAL) {   // update end of interval
    return;
  }
  sendAnalog(A0);
  Serial.print(F(","));
  sendAnalog(A1);
  Serial.println();
  lastMsg += MEASURE_INTERVAL;
} // end of doMeasurement()

void getCommand(__attribute__((unused)) unsigned long timeRef) {
  unsigned int inByte;
  if (Serial.available() <= 0) {   // is a serial byte ready?
    return; // No input available; just exit
  }

  inByte = Serial.read();
  switch(inByte) {              // what to do with this byte ?
    case 'C':
      digitalWrite(LED_PIN, HIGH);      // turn the LED on (HIGH is the voltage level)
      digitalWrite(ERROR_PIN, LOW);     // good char rx; clear any previous error status
      break;

    case 'c':
      digitalWrite(LED_PIN, LOW);       // turn the LED off (LOW is the voltage level)
      digitalWrite(ERROR_PIN, LOW);     // good char rx; clear any previous error status
      break;

    case '\n':     //ignore CR and LF
    case '\r':
      break;

    default:       // unknown char received
      digitalWrite(ERROR_PIN, HIGH);    // turn the LED on (HIGH indicates error)
      break;
  }  // end of switch
} // end of getCommand()

void loop() {
  unsigned long now = millis();
  doMeasurement(now);
  getCommand(now);
} // end of loop()
