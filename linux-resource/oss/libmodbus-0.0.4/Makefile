#
# FILE:
# Makefile
#
# FUNCTION:
# Makefile for modbus 

CC = arm-wrs-linux-gnueabi-armv6jel_vfp-uclibc_small-gcc
AR = arm-wrs-linux-gnueabi-armv6jel_vfp-uclibc_small-ar
NAME=libmodbus
VERSION=0.0.4

CFLAGS	= -Wall -O3

all: libmodbus

libmodbus: commun.o  mbm.o
	$(AR) rcs  libmodbus.a commun.o  mbm.o


commun.o: commun.c modbus.h
	$(CC) $(CFLAGS) -c commun.c -I.

mbm.o: mbm.c modbus.h 
	$(CC) $(CFLAGS) -c  mbm.c -I.

clean:
	rm -f  ~* *~ *.ASV *.bak *.o core> /dev/null

tar:	clean
	tar -C .. -c -v -z -f ../tar/$(NAME)-$(VERSION).tar.gz $(NAME)-$(VERSION)
