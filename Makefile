obj-m += hd44780.o
obj-m += lcd.o
lcd-objs := hd44780.o driver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) KBUILD_EXTMOD=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
