from uFire_EC import uFire_EC

# connect board SDA to pin 19, SCL to pin 23, but any pins can be used.
ec = uFire_EC(sda=19, scl=23)

ec.measureEC()
print("mS: " + str(ec.mS))
