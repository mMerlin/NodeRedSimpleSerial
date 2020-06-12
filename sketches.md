<!-- cSpell:enable -->
# Arduino Sketches

<link href="css/my_css.css" rel="stylesheet"/>

* [Analog Read Serial 01](#link_read_serial_01)
* [Analog Read Serial 02](#link_read_serial_02)
* [Analog Read Serial 03](#link_read_serial_03)
* [Analog Read Serial 04](#link_read_serial_04)

## <a name="link_read_serial_01">⚓</a> Analog Read Serial 01

[sketch file](sketchbook/AnalogReadSerial01/AnalogReadSerial01.ino)

Initial test sketch, based on [Analog Read Serial](http://www.arduino.cc/en/Tutorial/AnalogReadSerial) Tutorial example. Adjusted to read data from 2 analog pins, and to handle single character commands receive from the serial line.

Raw measurement values and calculated voltages are sent as human readable csv to the serial port. Here, human readable means a space after each comma.

A received "C" character turns the LED on pin 3 on, "c" turns it off. CR and LF characters are ignored. Any other character turns the built in LED on. Receiving a valid command character turns the built in LED off again.

This provides enough of an interface to create a Node-RED flow that receives and displays data from the Arduino, and to send commands that control what the Arduino is doing.

![serial flow](flows/initial%20serial%20flow.png "initial serial Flow")

## <a name="link_read_serial_02">⚓</a> Analog Read Serial 02

[sketch file](sketchbook/AnalogReadSerial02/AnalogReadSerial02.ino)

An adjusted sketch that more aggressively follows some coding conventions. Constant values are stored in `const` variables, with all upper case names. Literal values are move to named constants. Several signed variables have been changed to unsigned. The common measurement code has been refactored into a function. A global variable has been moved local to the single function that needs to reference it. The command processing code has been move to a separate function. Constant values and string literals have been moved to program memory. Change the csv output to remove the space after each comma, which allows the csv parser node to process it more cleanly.

## <a name="link_read_serial_03">⚓</a> Analog Read Serial 03

[sketch file](sketchbook/AnalogReadSerial03/AnalogReadSerial03.ino)

Prepare to restructure with using c++ classes for handling the separate functions of the sketch. Or at least simulate the way class methods are called and work with encapsulated data.

Make the final non-const global variable local and static in the only function the references it. Refactor the measurement functionality into a function that manages the interval internally. The loop function no longer needs and conditional logic. Instead it passes the current micros() time stamp to each called function, when lets them use elapsed time to control their internal state. For functions that do not actually use the time marker in their logic, add an unused attribute tag, to prevent the compiler from complaining about unused arguments.

## <a name="link_read_serial_04">⚓</a> Analog Read Serial 04

[sketch file](sketchbook/AnalogReadSerial04/AnalogReadSerial04.ino)

Refactor and expand the command code repertoire processed, to simulated a larger event loop handling independent, lossly coupled, functionality. Split the command reading from the command processing, and turn the processing of each command into a separate function. Turn the main loop function calls into an iterated array of function pointers, to simulated an environment where methods could be dynamically added to and removed from the main event processing loop.

The (from serial port) command reading method also handles error indication. It does not know anything about what commands are actually valid, but it detects commands that were not processed by any of the existing functions. Every iteration, every method gets a chance to see a received command, and respond to it. Even if processing the command will delayed until later, an interested party must save the command internally, and set a flag to show they have accepted the command.

With this structure, potentially multiple methods can initiate processing based on the same received command. It has not been implemented here, but something like a global (multiple methods) reset is possible.

<!--
* [Link](#link_link)
## <a name="link_link">⚓</a> Link
-->

<!-- cSpell:disable -->
<!-- cSpell:enable -->
<!--
# cSpell:disable
# cSpell:enable
cSpell:words
cSpell:ignore
cSpell:enableCompoundWords
-->
