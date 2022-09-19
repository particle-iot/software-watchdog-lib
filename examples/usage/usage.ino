// Example usage for AppWatchdogWorkaround library by Particle.

#include "SoftwareWatchdog.h"

// EXAMPLE USAGE
// Global variable to hold the watchdog object pointer
SoftwareWatchdog *wd = nullptr;

void watchdogHandler() {
  // Do as little as possible in this function, preferably just
  // calling System.reset().
  // Do not attempt to Particle.publish(), use Cellular.command()
  // or similar functions. You can save data to a retained variable
  // here safetly so you know the watchdog triggered when you 
  // restart.
  // In 2.0.0 and later, RESET_NO_WAIT prevents notifying the cloud of a pending reset
  System.reset(RESET_NO_WAIT);
}

void setup() {
  // Start watchdog. Reset the system after 60 seconds if 
  // the application is unresponsive.
  wd = new SoftwareWatchdog(60000, watchdogHandler, 1536);
}

void loop() {
  while (true) {
    SoftwareWatchdog::checkin(); // resets the AWDT count
  }
}
