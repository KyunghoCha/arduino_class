#include <Stepper.h>

const int stepsPerCycle = 2048;
Stepper myStepper(stepsPerCycle, 8, 10, 9, 11);

void setup() {
  myStepper.setSpeed(15);
}

void loop() {
  myStepper.step(stepsPerCycle);
  delay(500);

  myStepper.step(-stepsPerCycle);
  delay(500);
}
