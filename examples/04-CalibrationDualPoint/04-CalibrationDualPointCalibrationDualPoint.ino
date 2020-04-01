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
  
  // Go over the steps here: https://ufire.co/docs/uFire_EC/#dual-point
  // To follow the above steps as simply as possible, this example will be
  // compiled and ran twice. Once for the low point, then again
  // after commenting/uncommenting the lines below, for the high point

  // First, calibrate the low point
  // pass the mS value of the low calibration solution 
  // and that solution's temperature (the device's temperature sensor is used in this example)
  // https://ufire.co/docs/uFire_EC/api.html#calibrateprobelow

  ec.calibrateProbeLow(1.413, ec.measureTemp());
  // ^ comment this line after the low calibration

  // Second, calibrate the high point
  // pass the mS value of the high calibration solution 
  // and that solution's temperature
  // https://ufire.co/docs/uFire_EC/api.html#calibrateprobehigh

  // ec.calibrateProbeHigh(5.0, ec.measureTemp());
  // ^ uncomment this line after low calibration
}

void loop()
{
  // https://ufire.co/docs/uFire_EC/api.html#measureec
  Serial.println((String)"mS/cm: " + ec.measureEC());
  delay(1000);
}