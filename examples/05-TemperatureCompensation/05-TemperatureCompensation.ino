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

  // https://ufire.co/docs/uFire_EC/api.html#begin
  ec.begin(); 
}

void loop()
{
  // to take a temperature compensation measurement using the onboard temperature sensor
  // the default setting will adjust to 25 C
  // https://ufire.co/docs/uFire_EC/api.html#measuretemp
  ec.measureEC(ec.measureTemp());

  Serial.println((String)"mS/cm: " + ec.measureEC());
  delay(1000);
}
