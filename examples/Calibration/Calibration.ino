/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   It show's a user interactive dual point calibration routine.
 */

#include <uFire_EC.h>

uFire_EC ec;

void setup()
{
  Serial.begin(9600);

  // reset the configuration
  ec.reset();
  
  // use the EC value listed under the current temperature to calibrate
  // eg. if the temperature of the calibration solution is 20 C, use the
  // mS/cm value listed for that temperature
  ec.calibrateProbe(1.413);
}

void loop()
{
  ec.measureEC();
  Serial.print("mS/cm: ");
  Serial.println(ec.mS);
  delay(1000);
}