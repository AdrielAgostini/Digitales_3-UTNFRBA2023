NAME_MOD = I2Cdriver
obj-m := src/$(NAME_MOD).o
GREPN = SG

KERNELDIR = /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

#compila el modulo haciendo una llamada al Makefile que esta en '/lib/modules/$(shell uname -r)/build'
all: softclean build insmod

build:
	make -C $(KERNELDIR) M=$(PWD) modules

#limpia todos los archivos objetos
softclean:
	make -C $(KERNELDIR) M=$(PWD) clean

#instala el modulo
insmod:
	sudo insmod src/$(NAME_MOD).ko

#revisa si el modulo esta instalado
cat:
	cat /proc/modules | grep $(NAME_MOD)

clean: rmmod softclean

#desinstala el modulo
rmmod:
	sudo rmmod $(NAME_MOD)

#muestra los mensajes (dmesg), en el ejemplo todos los printk imprimen el nombre del archivo
dmesg:
	dmesg | grep $(GREPN) 

dmesgDato:
	dmesg | grep $(GREPN)dato 

info:
	modinfo src/$(NAME_MOD).ko

tail:
	tail -f /var/log/syslog