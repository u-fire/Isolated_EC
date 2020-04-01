from uFire_EC import uFire_EC

# Be sure to follow the directions here:
# https://ufire.co/docs/uFire_EC/#raspberry-pi

# For Raspberry Pi 3, 3 indicates the I2C-GPIO port
ec = uFire_EC(3)

# For Raspberry Pi 4, 7 indicates the I2C-GPIO port
#ec = uFire_EC(7)

ec.measureEC()
print("mS: " + str(ec.mS))
