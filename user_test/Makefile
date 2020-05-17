#!/bin/sh
CURR_DIR=$(shell pwd)
TARGET=usr_test
SRCS=$(wildcard *.c)

OBJS=$(SRCS:.c=.o)

all:$(TARGET)
$(TARGET): $(OBJS)
	$(CC) -O3 -o $(TARGET) $(OBJS)
install:
	$(HOST_INSTALL) $(TARGET) $(IMAGES_DIR)
clean:
	@rm -vf $(OBJS) $(TARGET)
.PHONY:all install clean
