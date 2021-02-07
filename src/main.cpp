#include "main.h"
#include "pros/apix.h"								// we need the advanced API header

#include "json.hpp"										// JSON parsing library for C++

#include <iostream>
#include <fstream>

// ----------------------- Configure serial console driver ---------------------
// See pros/src/system/dev/ser_driver.c for definitions
#define STDIN_STREAM_ID 0x706e6973   // 'sinp' little endian
#define STDOUT_STREAM_ID 0x74756f73  // 'sout' little endian
#define STDERR_STREAM_ID 0x72726573  // 'serr' little endian
#define KDBG_STREAM_ID 0x6762646b    // 'kdbg' little endian

int portID = STDOUT_STREAM_ID;			// what serial port are we going to set
																		// options for?
// --------------------- END serial driver configuration -----------------------

std::string messageString;

using json = nlohmann::json;				// set JSON name space for use

// -------------------- Define thread/task functions ---------------------------
void messageRxTask(void* ignore) {
	 //the void* is there to provide a way to pass a

	std::cout << "V5: Starting Message Receive Task \n";

  // Task loop make sure you do not starve the processor
	while(true) {
 		 std::getline(std::cin, messageString);
		 std::cout << "V5: Got line: " << messageString << "\n";
		 pros::delay(20);  // run a 50HZ
	}
}

// ---------------- END trhead defintions --------------------------------------

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	// Setting up serial console through USB port for use with RPI Zero W radio
	std::cout << "Before port reconfiguration \n";
	std::cout << "portID " << &portID << "\n";
	pros::c::serctl(SERCTL_DEACTIVATE, &portID);		// deactivate serr and sout
																									// prefixes in the stream
	pros::c::serctl(SERCTL_DISABLE_COBS, &portID);	// disable byte stuffing COBS

}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
  // start he message recieve task, so we can receive data from the RPI Zero via the serial
	// interface.  This task runs at 20ms interval - or 50HZ
	pros::Task messageRecieveTask(messageRxTask, (void*)"PROS", TASK_PRIORITY_DEFAULT,
                TASK_STACK_DEPTH_DEFAULT, "Message Receiver Task"); //starts the task
	// no need to provide any other parameters

  // ------------------ JSON testing code --------------------------------------
	json message;											// declare a json object message
	message["pi"]=3.141;
	message["happy"] = true;
	message["name"] = "willem";
	message["nothing"] = nullptr;
  message["answer"]["everything"] = 42;
  // ---------------- END JSON test defenition ---------------------------------

	std::string mystr;
  // activate write mode by sending over the keyword
  std::cout << "writemode" << "\n";
  int counter = 0;

	while (true) {
		/// Write a messagge to the RPI Zero
    // message MSUT end with a '\n' in order to transmit
    std::cout << "V5: writing loop - counter: " << counter++ << "\n";

    // ------------------------ Write serialized JSON object to console --------
    // write JSON object to cout - serialized
    std::cout << message << "\n";
    // ------------------------ END serialized JSON write ----------------------

		pros::delay(500);   // write message for now at 2HZ
	}
}
