/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   For hardware version 2, firmware 3
 */

#include <uFire_EC.h>
uFire_EC ec;

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  // start the EC board
  //https://ufire.co/docs/uFire_EC/api.html#begin
  ec.begin();
  
  // reset the calibration data
  //https://ufire.co/docs/uFire_EC/api.html#reset
  ec.reset();
  
  // mS/cm value of the calibration solution. Use the labeled value, not
  // a temperature-adjusted value. 
  // this example assumes the device has a temperature sensor attached
  // https://ufire.co/docs/uFire_EC/api.html#calibrateprobe
  ec.calibrateProbe(1.413, ec.measureTemp());
}

void loop()
{
  // https://ufire.co/docs/uFire_EC/api.html#measureec
  Serial.println((String)"mS/cm: " + ec.measureEC());
  delay(1000);
}