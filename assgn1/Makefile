obj-m += adxl.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -g ./userapp.c -o app

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm ./app

app: userapp.c
	gcc -g ./userapp.c -o app	