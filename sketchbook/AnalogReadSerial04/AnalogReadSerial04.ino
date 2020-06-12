/*
  AnalogReadSerial HPD 2020-06-11

  Reads an analog input on pin A0 and A1, prints the result to the Serial Monitor.
  Watch serial input and change LED state for specific 'command' characters
  Expand the command codes processed to dynamically adjust the measurement interval

  Each method is responsible for its own timing
  Extra commands added to show how the structure can be extended
*/

const PROGMEM unsigned int LED_PIN = 3; // pin digital pin controlling LED state
const PROGMEM unsigned int ERROR_PIN = LED_BUILTIN; // show that an unrecognized command was received
const PROGMEM unsigned long BAUD_RATE = 9600; // serial communication speed
const PROGMEM float MAX_VOLTAGE = 5.0; // voltage the board is running at
const PROGMEM unsigned int MAX_MEASURE = 1024; // maximum analog reading
const PROGMEM unsigned long DEFAULT_INTERVAL = 5000; // 5 seconds
const PROGMEM unsigned long INTERVAL_DELTA = 1000;   // +/- 1 second
const PROGMEM unsigned long MAX_INTERVAL = 10000;    // 10 seconds
const PROGMEM unsigned long MIN_INTERVAL = 1000;     // 1 second

boolean commandUsed = true;
boolean newCommand = false;
unsigned int commandByte;
unsigned long measureInterval = DEFAULT_INTERVAL;
unsigned long newInterval = DEFAULT_INTERVAL;

void sendAnalog(int sourcePin) { // output as csv, no white space
  // get the current value for the specified source pin
  int sensorValue = analogRead(sourcePin);
  // print (to serial) the measured value
  Serial.print(sensorValue);
  Serial.print(F(","));
  // convert the measurement to a voltage
  float voltage = sensorValue * MAX_VOLTAGE / MAX_MEASURE;
  Serial.print(voltage);
} // end of sendAnalog()

void getCommand(__attribute__((unused)) const unsigned long timeRef) {
  if (newCommand) {
    // A new command was published last call
    if (!commandUsed) {
      // The published command was not recognized by any one
      digitalWrite(ERROR_PIN, HIGH);    // turn the LED on (HIGH indicates error)
      commandUsed = true;
    } else {
      digitalWrite(ERROR_PIN, LOW);     // good char rx; clear any previous error status
    }
    newCommand = false; // Make sure a single command does not get processed twice
  }

  if (Serial.available() <= 0) {   // is a serial byte ready?
    return; // No input available; just exit
  }

  commandByte = Serial.read();
  if (commandByte != '\n' && commandByte != '\r') { // ignore CRLF
    newCommand = true;
    commandUsed = false;
  }
} // end of getCommand()

void setLED(__attribute__((unused)) const unsigned long timeRef) {
  if (!newCommand || commandByte != 'C') {
    return;
  }

  digitalWrite(LED_PIN, HIGH);      // turn the LED on (HIGH is the voltage level)
  commandUsed = true;               // show that the command was recognized and processed
} // end of setLED()

void clearLED(__attribute__((unused)) const unsigned long timeRef) {
  if (!newCommand || commandByte != 'c') {
    return;
  }

  digitalWrite(LED_PIN, LOW);       // turn the LED off (LOW is the voltage level)
  commandUsed = true;               // show that the command was recognized and processed
} // end of clearLED()

void increaseInterval(__attribute__((unused)) const unsigned long timeRef) {
  if (!newCommand || commandByte != '+') {
    return;
  }
  commandUsed = true;               // show that the command was recognized and processed
  if (newInterval < MAX_INTERVAL) {
    newInterval += INTERVAL_DELTA;
  }
} // end of increaseInterval()

void decreaseInterval(__attribute__((unused)) const unsigned long timeRef) {
  if (!newCommand || commandByte != '-') {
    return;
  }

  commandUsed = true;               // show that the command was recognized and processed
  if (newInterval > MIN_INTERVAL) {
    newInterval -= INTERVAL_DELTA;
  }
} // end of decreaseInterval()

void resetInterval(__attribute__((unused)) const unsigned long timeRef) {
  if (!newCommand || commandByte != 'r') {
    return;
  }

  commandUsed = true;               // show that the command was recognized and processed
  newInterval = DEFAULT_INTERVAL;
} // end of resetInterval()

void doMeasurement(const unsigned long timeRef) {
  static unsigned long lastMsg = 0;  // message send timer
  if (timeRef - lastMsg < measureInterval) {
    return; // nothing to do at this time
  }

  sendAnalog(A0);
  Serial.print(F(","));
  sendAnalog(A1);
  Serial.println();

  measureInterval = newInterval; // Only change at end of interval
  lastMsg += measureInterval;
  while (lastMsg > timeRef) {
    // prevent interval changes from breaking the synchronization
    lastMsg -= INTERVAL_DELTA;
  }
} // end of doMeasurement()

void (*queue[])(const unsigned long) = {
  &doMeasurement,
  &getCommand,
  &setLED,
  &clearLED,
  &increaseInterval,
  &decreaseInterval,
  &resetInterval
};
const unsigned int queueSize = sizeof(queue) / sizeof(queue[0]);

void setup() {
  // initialize digital output pins
  pinMode(ERROR_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(BAUD_RATE);     // initialize serial communication
} // end of setup()

void loop() {
  unsigned long now = millis();
  // Let each method decide when it needs to actually do something.
  for (unsigned int i = 0; i < queueSize; i++) {
    (queue[i])(now);
  }
} // end of loop()
