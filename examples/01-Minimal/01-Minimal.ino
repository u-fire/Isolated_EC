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
  // https://ufire.co/docs/uFire_EC/api.html#measureec
  Serial.print((String) "mS/cm: " + ec.measureEC());
  delay(1000);
}