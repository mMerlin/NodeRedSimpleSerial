/*
  AnalogReadSerial DG 2020-06-10

  Reads analog input on pin A0 and A1, prints the result to the Serial Monitor.

  http://www.arduino.cc/en/Tutorial/AnalogReadSerial
*/

long lastMsg = 0;     //message send timer
int inByte = 0;       // incoming serial byte
int ledPin = 3;       // pin D3

// the setup routine runs once when you press reset:
void setup() {
    // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ledPin, OUTPUT);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

void senddata() {
  // read the input on analog pin 0:
  int sensorValue0 = analogRead(A0);
  // print out the value you read:
  Serial.print(sensorValue0);
  Serial.print(", ");
  // convert the sensor reading to voltage
  float voltage0 = sensorValue0 * 5.0 / 1024;
  Serial.print(voltage0);
  Serial.print(", ");

  // read the input on analog pin 1:
  int sensorValue1 = analogRead(A1);
  // print out the value you read:
  Serial.print(sensorValue1);
  Serial.print(", ");
  // convert the sensor reading to voltage
  float voltage1 = sensorValue1 * 5.0 / 1024;
  Serial.println(voltage1);
}

// the loop routine runs over and over again forever:
void loop() {
  long now = millis();
  if (now - lastMsg > 5000) {   // update every five seconds
    lastMsg = now;
    senddata();
  }

  if (Serial.available() > 0) {   // is a serial byte ready?
                                  // get incoming byte:
    inByte = Serial.read();

    switch(inByte) {              // what to do with this byte ?
      case 'C':
        digitalWrite(ledPin, HIGH);     // turn the LED on (HIGH is the voltage level)
        digitalWrite(LED_BUILTIN, LOW);   // good char rx
        break;

      case 'c':
        digitalWrite(ledPin, LOW);    // turn the LED off (HIGH is the voltage level)
        digitalWrite(LED_BUILTIN, LOW); // good char rx
        break;

      case '\n':        //ignore CR and LF
      case '\r':
        break;

      default:      // unknown char received
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH indicates error)
        break;

     }  // end of switch
   }    // end of serial avail
}       // end of loop
