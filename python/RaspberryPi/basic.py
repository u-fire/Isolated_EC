from uFire_EC import uFire_EC

# the passed parameter 3 indicates the third (/dev/i2c-3) I2C device
ec = uFire_EC()

ec.measureEC()
print("mS: " + str(ec.mS))
