/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone
   
   It demonstrates the minimal amount of code and setup to take a reading
   using temperature compensation.
 */
#include <uFire_EC.h>

uFire_EC ec;

void setup()
{
  Serial.begin(9600);

  // the default temperature constant is 25 C. This is the temperature the measurement
  // will be adjusted to. To change it, you can call setTempConstant(float) or see below
  // for the other way
  ec.setTempConstant(20);

  // the default temperature coefficient is 0.019. This is the factor used when 
  // adjusting the EC reading for temperature. You probably won't want to change it, 
  // but you can by calling:
  ec.setTempCoefficient(0.019);
  
}

void loop()
{
  // to take a temperature compensation measurement using the onboard temperature sensor
  // the default setting will adjust to 25C unless it's been changed. 
  ec.measureEC(ec.measureTemp());

  // to adjust to a different temperature pass it as the second parameter:
  ec.measureEC(ec.measureTemp(), 20);

  Serial.print("mS/cm: ");
  Serial.println(ec.mS);
  Serial.println("-----");
  delay(1000);
}
