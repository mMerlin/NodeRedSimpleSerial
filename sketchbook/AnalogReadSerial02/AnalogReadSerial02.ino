/*
  AnalogReadSerial HPD 2020-06-11

  Reads analog input on pin A0 and A1, prints the result to the Serial Monitor.
  Watch serial input and change LED state for specific 'command' characters

  http://www.arduino.cc/en/Tutorial/AnalogReadSerial
*/

const PROGMEM unsigned int LED_PIN = 3; // pin digital pin controlling LED state
const PROGMEM unsigned int ERROR_PIN = LED_BUILTIN; // show that an unrecognized command was received
const PROGMEM unsigned long BAUD_RATE = 9600; // serial communication speed
const PROGMEM float MAX_VOLTAGE = 5.0; // voltage the board is running at
const PROGMEM unsigned int MAX_MEASURE = 1024; // maximum analog reading
const PROGMEM unsigned long MEASURE_INTERVAL = 5000; // 5 seconds

unsigned long lastMsg = 0;     // message send timer

void setup() {
  // initialize used digital output pins
  pinMode(ERROR_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(BAUD_RATE);     // initialize serial communication
} // end of setup()

void senddata() {
  // do measurements and log results as csv to serial
  sendAnalog(A0);
  Serial.print(F(","));
  sendAnalog(A1);
  Serial.println();
} // end of senddata()

void sendAnalog(int sourcePin) {
  // get the current value for the specified source pin
  int sensorValue = analogRead(sourcePin);
  Serial.print(sensorValue);
  Serial.print(F(","));
  // convert the measurement to a voltage
  float voltage = sensorValue * MAX_VOLTAGE / MAX_MEASURE;
  Serial.print(voltage);
} // end of sendAnalog()

void getCommand() {
  unsigned int inByte;
  if (Serial.available() <= 0) {   // is a serial byte ready?
    return; // No input available; nothing to do here yet
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
  if (now - lastMsg > MEASURE_INTERVAL) {   // update every five seconds
    // lastMsg = now; // can 'drift'
    lastMsg += MEASURE_INTERVAL; // no drifting
    senddata();
  }
  getCommand();
} // end of loop()
