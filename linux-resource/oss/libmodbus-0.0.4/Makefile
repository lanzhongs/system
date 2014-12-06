#
# FILE:
# Makefile
#
# FUNCTION:
# Makefile for modbus 

CC = arm-wrs-linux-gnueabi-armv6jel_vfp-uclibc_small-gcc

NAME=libmodbus
VERSION=0.0.4

CFLAGS	= -Wall -O3

all: libmodbus

libmodbus: commun.o  mbm.o
	$(CC) $(CFLAGS) -o libmodbus.so commun.o  mbm.o -lpthread  -shared -I.


commun.o: commun.c modbus.h
	$(CC) $(CFLAGS) -c commun.c -I.

mbm.o: mbm.c modbus.h 
	$(CC) $(CFLAGS) -c  mbm.c -I.

clean:
	rm -f  ~* *~ *.ASV *.bak *.o core> /dev/null

tar:	clean
	tar -C .. -c -v -z -f ../tar/$(NAME)-$(VERSION).tar.gz $(NAME)-$(VERSION)
